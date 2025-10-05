#ifndef SERVER_H
#define SERVER_H

#define lfs_METADATA_SEPARATOR_TOKEN "\n\n"

extern const int listen_backlog;

typedef enum
{
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
} lfs_Method;

typedef struct
{
    const char* route;
    lfs_Method method;
    const char* version;
    size_t content_length;
} lfs_RequestMetadata;

int lfs_listen(const char* host, const char* port);

#endif //SERVER_H
