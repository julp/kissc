#ifndef SIGN_H

# define sign(x) (((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))

#if 0
# define sign(x) \
    ({ int _x = (x); _x > 0 ? 1 : (_x < 0 ? -1 : 0); })
#endif

#endif /* !SIGN_H */
