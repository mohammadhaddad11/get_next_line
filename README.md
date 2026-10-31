*This project was created as part of the 42 curriculum by mhaddad.*

# get_next_line

`get_next_line` reads text from a file descriptor and returns one allocated line
per call. A line may span several reads, and one read may contain several lines.
The implementation bridges that mismatch by retaining unused bytes between calls.

The public contract is small:

```c
char *get_next_line(int fd);
```

A successful call returns a NUL-terminated string, including its newline when
present. A final line without newline is still returned. NULL means EOF or an
error. The caller owns every returned line and must free it.

## Build and run

Requirements: a C compiler, Make, and `ar` on a Unix-like system.

```sh
git clone git@github.com:mohammadhaddad11/get_next_line.git
cd get_next_line
make
make example
./gnl_example README.md
```

The archive is `libgnl.a`. The [example driver](examples/read_lines.c) opens its
filename argument, repeatedly calls the function, prints/frees each result,
then closes the descriptor. It reproduces the input text rather than adding a
new newline after every returned string.

`make clean` removes objects; `make fclean` also removes the archive and example;
`make re` rebuilds. The default `BUFFER_SIZE` is **42**. To compile the driver
directly with a four-byte read buffer:

```sh
cc -Wall -Wextra -Werror -DBUFFER_SIZE=4 -I. \
  examples/read_lines.c get_next_line.c get_next_line_utils.c -o gnl_example
```

For the archive, use `make re CPPFLAGS=-DBUFFER_SIZE=4`; changing a flag requires
rebuilding objects, not merely relinking a driver against an old archive.

## The reading lifecycle

```text
file descriptor → read buffer → static stash → allocated line → caller frees
                                    │
                                    └── unread suffix → next call
```

### 1. Validate the descriptor

[`get_next_line`](get_next_line.c) rejects a negative descriptor, a nonpositive
buffer size, or failure of a zero-byte `read` probe. It then passes the address
of its static stash pointer to `fill` so that helper can replace the retained
allocation as data arrives.

### 2. Fill only until a complete line is available

`fill` allocates a temporary `BUFFER_SIZE + 1` buffer. It looks for a newline
using `ft_strnl`; if none is present, it calls `read`, terminates the bytes read,
and appends them to the stash. It repeats until newline, EOF, or read failure.
An already buffered newline avoids another data read.

### 3. Allocate the returned line

`line_len` chooses the first newline plus one byte, or the full stash length
when no newline exists. `extract` allocates exactly that length plus a NUL
terminator and copies the selected prefix. The returned line is independent
of the stash allocation.

### 4. Keep what belongs to later calls

`clean` copies the unread suffix into a new allocation, then frees the old
stash. If there is no remainder, it frees the stash and resets it to NULL.
The static pointer survives the function return, which is how the next call
can start with bytes already consumed from the descriptor.

## A concrete four-call example

Suppose a freshly opened file contains `first\nsecond\nlast`, with
`BUFFER_SIZE=4`:

| Call | New chunks needed | Returned allocation | Retained stash |
| --- | --- | --- | --- |
| 1 | `firs`, `t\nse` | `first\n` | `se` |
| 2 | `cond`, `\nlas` | `second\n` | `las` |
| 3 | `t`, then EOF | `last` | None |
| 4 | EOF | NULL | None |

The first call cannot return after reading only `firs`: the line is incomplete.
It also cannot discard `se`: the descriptor has already advanced beyond those
bytes. The final `last` must be returned once before NULL, even without a newline.

A client loop follows that ownership lifecycle:

```c
char *line = get_next_line(fd);
while (line != NULL)
{
    /* Use line here; it already includes '\n' if the input had one. */
    free(line);
    line = get_next_line(fd);
}
```

## Memory and helper contracts

The stash is a C string, not a linked list of chunks. The private
[`ft_strjoin`](get_next_line_utils.c) allocates the combined string and **frees
its first argument**. This permits replacing the stash without leaking its
previous allocation, but differs from standalone libft's same-named function.

Several helper names also overlap libft's exported symbols. Combining both
libraries in one application requires checking those interfaces rather than
assuming identically named routines are interchangeable.

The temporary read buffer is freed by `fill`; old stash memory is freed during
joining, cleanup, or the main error path; returned lines belong to the caller.
Repeatedly joining and rescanning growing text can take quadratic time for a
long line. `BUFFER_SIZE` controls read chunk size, not maximum line length.

## Design boundaries and checks

There is **one static stash**, not one per descriptor. Interleaving files can
mix their buffered content; the implementation is neither reentrant nor
thread-safe. It targets text strings, not arbitrary binary input with NUL bytes.

NULL does not distinguish clean EOF from read/allocation failure, so the sample
driver cannot report that distinction reliably. An early invalid-descriptor
return does not clear a previous stash. Stopping before EOF can retain buffered
memory with no explicit cleanup API, and suffix-allocation failure can discard
unread bytes after a line has already been extracted.

Strict compilation and representative roundtrips passed with buffer sizes 1,
4, and 42, including empty text, a long line, and a final unterminated line.
These checks support the normal lifecycle above, not a multi-descriptor bonus
implementation or exhaustive allocation-failure guarantees.

## Resources

- Manual pages: `read(2)`, `open(2)`, `close(2)`, `malloc(3)`
- [C storage duration](https://en.cppreference.com/w/c/language/storage_duration)
- [File I/O learning video](https://www.youtube.com/watch?v=8E9siq7apUU&t=1207s)

## AI Usage

The existing disclosure records AI help with learning/planning, descriptors,
static state, memory handling, debugging, and documentation. AI also assisted
the example driver and verification material. This README explains the
preserved line-reading implementation rather than replacing it.
