#ifndef SERVER_H
#define SERVER_H

// Macros used for parsing the incoming request data
#define lfs_READMETADATA 0
#define lfs_READBODY 1

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

size_t lfs_listen(const char* host, const char* port);

void lfs_accept(int sockfd);

/**
 * This function expects buf to be null terminated
 * @param buf
 * @param metadata
 * @param content
 * @param read_mode
 * @return
 */
int lfs_parse_request(const char* buf, size_t buf_len, lfs_RequestMetadata* metadata, char* content, short* read_mode);

#endif //SERVER_H
