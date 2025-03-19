#include "http_server.h"
#include <fstream>
void HttpServer::echo_message_cb(const TcpConnSP &conn, InputBuffer *ibuf)
{
    const char *msg = ibuf->get_from_buf();
    string msg_str(msg, msg + ibuf->length());
    ibuf->pop(ibuf->length());
    ibuf->adjust();
    PR_INFO("socket fd %d recv message:%s", conn->get_fd(), msg_str.c_str());

    HttpMessage hmsg;
    hmsg.parse_request_message(msg_str);
    deal_method(conn,hmsg);
    // hmsg.print_message();
}

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        PR_WARN("file %s no exist.\n",path.c_str());
        return "";
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void HttpServer::deal_method(const TcpConnSP &conn,HttpMessage &hmsg)
{
    if (hmsg.request.method == "GET")
    {
        string file_path = "."+hmsg.request.path;
        string content = read_file(file_path);
        if (content.empty()) {
            string nofind = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            conn->send(nofind.c_str(),nofind.length());
        }
        hmsg.build_response_message("HTTP/1.1","200","OK",content);
    }
    else if (hmsg.request.method == "POST")
    {
    }
    else
    {
    }
    conn->send(hmsg.response.response_seq.c_str(),hmsg.response.response_seq.length());
}

void HttpMessage::parse_request_message(string &msg)
{
    std::istringstream stream(msg);
    string line;
    string key, value;
    bool isFirstline = true;
    PR_DEBUG("start parse\n");
    while (std::getline(stream, line))
    {
        // 去除行末尾的 \r（如果有）
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (isFirstline)
        {
            PR_DEBUG("start parse first line\n");
            std::istringstream linestream(line);
            linestream >> request.method >> request.path >> request.protocol;
            isFirstline = false;
            if (request.path == "/")
                request.path = "/index.html";
        }
        else
        {
            PR_DEBUG("start parse headers\n");
            size_t separator_pos = line.find(": ");
            if (separator_pos != string::npos)
            {
                key = line.substr(0, separator_pos);
                value = line.substr(separator_pos + 2);
                request.headers.emplace(std::move(key), std::move(value));
            }
        }
    }
}

void HttpMessage::print_message() const
{
    cout << request.method << " " << request.path << " " << request.protocol << endl;
    for (auto &[k, v] : request.headers)
    {
        cout << k << ": " << v << endl;
    }
}

void HttpMessage::build_response_message(string protocol ,string status_code ,string status_message,string &content){
    response.protocol = protocol;
    response.status_code = status_code;
    response.status_message = status_message;
    response.contentType = "text/html";
    response.contentLength = std::to_string(content.size());
    response.databuf = content;

    response.response_seq = protocol + " " + status_code + " " + status_message + "\r\n";
    response.response_seq+= "Content-Length: " + response.contentLength + "\r\n";
    response.response_seq+= "Content-Type: "+ response.contentType + "\r\n";
    response.response_seq+= "\r\n";
    response.response_seq+= content;
}