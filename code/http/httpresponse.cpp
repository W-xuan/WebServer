#include "httpresponse.h"

namespace MicroWS
{
    const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
        {".html", "text/html"},
        {".xml", "text/xml"},
        {".xhtml", "application/xhtml+xml"},
        {".txt", "text/plain"},
        {".rtf", "application/rtf"},
        {".pdf", "application/pdf"},
        {".word", "application/nsword"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".au", "audio/basic"},
        {".mpeg", "video/mpeg"},
        {".mpg", "video/mpeg"},
        {".avi", "video/x-msvideo"},
        {".gz", "application/x-gzip"},
        {".tar", "application/x-tar"},
        {".css", "text/css "},
        {".js", "text/javascript "},
    };

    const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
        {200, "OK"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
    };

    const std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
        {400, "/400.html"},
        {403, "/403.html"},
        {404, "/404.html"},
    };

    HttpResponse::HttpResponse()
    {
        code_ = -1;
        path_ = srcDir_ = "";
        isKeepAlive_ = false;
        mmFile_ = nullptr;
        mmFileStat_ = {0};
    };

    HttpResponse::~HttpResponse()
    {
        UnmapFile();
    }

    void HttpResponse::Init(const std::string &srcDir, std::string &path, bool isKeepAlive, int code)
    {
        assert(srcDir != "");
        if (mmFile_)
        {
            UnmapFile();
        }
        code_ = code;
        isKeepAlive_ = isKeepAlive;
        path_ = path;
        srcDir_ = srcDir;
        mmFile_ = nullptr;
        mmFileStat_ = {0};
    }

    void HttpResponse::MakeResponse(Buffer &buff)
    {
        if (stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode))
        {
            code_ = 404;
        }
        else if (!(mmFileStat_.st_mode & S_IROTH))
        {
            code_ = 403;
        }
        else if (code_ == -1)
        {
            code_ = 200;
        }
        ErrorHtml_();
        AddStateLine_(buff);
        AddHeader_(buff);
        AddContent_(buff);
    }

    char *HttpResponse::File()
    {
        return mmFile_;
    }

    size_t HttpResponse::FileLen() const
    {
        return mmFileStat_.st_size;
    }

    void HttpResponse::ErrorHtml_()
    {
        if (CODE_PATH.count(code_) == 1)
        {
            path_ = CODE_PATH.find(code_)->second;
            stat((srcDir_ + path_).data(), &mmFileStat_);
        }
    }

    void HttpResponse::AddStateLine_(Buffer &buff)
    {
        std::string status;
        if (CODE_STATUS.count(code_) == 1)
        {
            status = CODE_STATUS.find(code_)->second;
        }
        else
        {
            code_ = 400;
            status = CODE_STATUS.find(400)->second;
        }
        buff.Append("HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n");
    }

    void HttpResponse::AddHeader_(Buffer &buff)
    {
        buff.Append("Connection: ");
        if (isKeepAlive_)
        {
            buff.Append("keep-alive\r\n");
            buff.Append("keep-alive: max=6, timeout=120\r\n");
        }
        else
        {
            buff.Append("close\r\n");
        }
        buff.Append("Content-type: " + GetFileType_() + "\r\n");
    }
}