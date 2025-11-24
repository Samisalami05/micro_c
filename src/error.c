#include <error.h>

char* error_str(error e) {
	switch (e) {
		case ERROR_FAILURE: return "ERROR Failure";
		case ERROR_OOM: return "ERROR OutOfMemory";
		case ERROR_LEXER: return "ERROR Lexer";
		case ERROR_PARSER: return "ERROR Parser";
		default: return "ERROR Success";
	}
}
