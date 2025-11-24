#ifndef LOGGER_H
#define LOGGER_H

#define COL_HEADER "\033[95m"
#define COL_OKBLUE "\033[94m"
#define COL_OKCYAN "\033[96m"
#define COL_OKGREEN "\033[92m"
#define COL_WARNING "\033[93m"
#define COL_FAILURE "\033[91m"
#define COL_ENDC "\033[0m"
#define COL_BOLD "\033[1m"
#define COL_UNDERLINE "\033[4m"

#define LOG_HEADER(header, msg, ...) fprintf(stderr, "%s%s: <%d> %s%s:%s " msg , COL_HEADER, __FILE_NAME__, __LINE__, COL_OKCYAN, header, COL_ENDC, ##__VA_ARGS__)
#define LOG_MSG(msg, ...) fprintf(stderr, "%s%s: <%d> %sMSG:%s " msg , COL_HEADER, __FILE_NAME__, __LINE__, COL_OKBLUE, COL_ENDC, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) fprintf(stderr, "%s%s: <%d> %sWARNING:%s " msg , COL_HEADER, __FILE_NAME__, __LINE__, COL_WARNING, COL_ENDC, ##__VA_ARGS__)
#define LOG_ERR(msg, ...) fprintf(stderr, "%s%s: <%d> %sERROR:%s " msg , COL_HEADER, __FILE_NAME__, __LINE__, COL_FAILURE, COL_ENDC, ##__VA_ARGS__)
#define LOG_ERROR(type, msg, ...) fprintf(stderr, "%s%s: <%d> %s%s:%s " msg , COL_HEADER, __FILE_NAME__, __LINE__, COL_FAILURE, error_str(type), COL_ENDC, ##__VA_ARGS__)

#define PRINT_HEADER(header, msg, ...) fprintf(stderr, "%s%s:%s " msg , COL_OKCYAN, header, COL_ENDC, ##__VA_ARGS__)
#define PRINT_MSG(msg, ...) fprintf(stderr, "%sMSG:%s " msg , COL_OKBLUE, COL_ENDC, ##__VA_ARGS__)
#define PRINT_WARNING(msg, ...) fprintf(stderr, "%sWARNING:%s " msg , COL_WARNING, COL_ENDC, ##__VA_ARGS__)
#define PRINT_ERR(msg, ...) fprintf(stderr, "%sERROR:%s " msg , COL_FAILURE, COL_ENDC, ##__VA_ARGS__)

#endif
