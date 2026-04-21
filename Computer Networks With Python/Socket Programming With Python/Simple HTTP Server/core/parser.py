def parse_request(request_text):
    lines = request_text.splitlines()
    if not lines:
        return None

    parts = lines[0].split()
    if len(parts) != 3:
        return None

    method, path, version = parts
    return {
        "method": method,
        "path": path,
        "version": version,
    }