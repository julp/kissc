#ifndef ATTRIBUTES_H

# define ATTRIBUTES_H

# ifdef __GNUC__
#  define GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
# else
#  define GCC_VERSION 0
# endif /* __GNUC__ */

# ifndef __has_attribute
#  define __has_attribute(x) 0
# endif /* !__has_attribute */

# if __GNUC__ || __has_attribute(unused)
#  define UNUSED(x) UNUSED_ ## x __attribute__((unused))
# else
#  define UNUSED
# endif /* UNUSED */

# if GCC_VERSION >= 3004 || __has_attribute(warn_unused_result)
#  define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
# else
#  define WARN_UNUSED_RESULT
# endif /* WARN_UNUSED_RESULT */

# if GCC_VERSION >= 2096 || __has_attribute(malloc)
#  define MALLOC __attribute__((malloc))
# else
#  define MALLOC
# endif /* MALLOC */

# if GCC_VERSION >= 4003
#  define ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
# else
#  define ALLOC_SIZE(...)
# endif /* ALLOC_SIZE */

# if (GCC_VERSION >= 3003 || __has_attribute(nonnull))
#  define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
# else
#  define NONNULL(...)
# endif /* NONNULL */

# if GCC_VERSION >= 2003 || __has_attribute(format)
#  define FORMAT(archetype, string_index, first_to_check) __attribute__((format(archetype, string_index, first_to_check)))
#  define PRINTF(string_index, first_to_check) FORMAT(__printf__, string_index, first_to_check)
# else
#  define FORMAT(archetype, string_index, first_to_check)
#  define PRINTF(string_index, first_to_check)
# endif /* FORMAT,PRINTF */

#endif /* !ATTRIBUTES_H */
