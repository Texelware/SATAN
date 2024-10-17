#ifndef STATUS_H
#define STATUS_H


#define SATAN_ALL_OK 0
#define EIO 1
#define EINVARG 2
#define ENOMEM 3

#define TRY(op) do { int result = op; if (result != 0) return result; } while (0)

#endif
