#ifndef LOGGER_H
#define LOGGER_H

namespace lfs
{
#if LFS_LOG_LEVEL>=1
#define LFS_LOG_CRITICAL(msg, args...) \
printf("[ CRITICAL ]: "); \
printf(msg, args)
#endif

#if LFS_LOG_LEVEL>=2
#define LFS_LOG_ERROR(msg, args...) \
printf("[ ERROR ]: "); \
printf(msg, args)
#endif

#if LFS_LOG_LEVEL>=3
#define LFS_LOG_WARNING(msg, args...) \
printf("[ WARNING ]: "); \
printf(msg, args)
#endif

#if LFS_LOG_LEVEL>=4
#define LFS_LOG_INFO(msg, args...) \
printf("[ INFO ]: "); \
printf(msg, args)
#endif

#if LFS_LOG_LEVEL>=5
#define LFS_LOG_DEBUG(msg, args...) \
printf("[ DEBUG ]: "); \
printf(msg, args)
#endif
}

#endif //LOGGER_H
