import mimetypes


STATUS_TEXT = {
    200: "OK",
    404: "Not Found",
}


def guess_content_type(file_path):
    content_type, _ = mimetypes.guess_type(file_path)
    if content_type and content_type.startswith("text/"):
        return f"{content_type}; charset=utf-8"
    return content_type or "application/octet-stream"


def build_response(status_code, body, content_type):
    reason = STATUS_TEXT[status_code]
    headers = [
        f"HTTP/1.1 {status_code} {reason}",
        f"Content-Type: {content_type}",
        f"Content-Length: {len(body)}",
        "Connection: close",
        "",
        "",
    ]
    return "\r\n".join(headers).encode("utf-8") + body