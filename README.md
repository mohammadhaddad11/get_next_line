*This project was created as part of the 42 curriculum by mhaddad.*

# get_next_line

## Description

Read a text stream one line at a time from a file descriptor. Each successful
call returns a newly allocated string, retaining a trailing newline when one
exists. A static stash preserves bytes already read beyond the returned line.
The project exercises descriptor I/O, persistent state, dynamic allocation,
and ownership across repeated function calls.

## Instructions

Requirements: a C compiler, Make, and `ar` on a Unix-like system.

```sh
git clone git@github.com:mohammadhaddad11/get_next_line.git
cd get_next_line
make
make example
./gnl_example README.md
```

The publication Makefile builds `libgnl.a`; the example links a real `main`
from [`examples/read_lines.c`](examples/read_lines.c). It opens the supplied
file, prints each returned line, frees it, and closes the descriptor.
`clean`, `fclean`, and `re` are available. `fclean` also removes the example.

The original function can also be compiled directly with your own driver:

```sh
cc -Wall -Wextra -Werror -DBUFFER_SIZE=42 -I. \
  examples/read_lines.c get_next_line.c get_next_line_utils.c -o gnl_example
```

Use `make re CPPFLAGS=-DBUFFER_SIZE=1` to rebuild the archive with another buffer
size. The default in the header is 42. Rebuild when changing compilation flags.

## Technical Implementation

```c
char *get_next_line(int fd);
```

1. Validate the descriptor and buffer size, including a zero-byte `read` probe.
2. Fill the static stash in chunks until newline, EOF, or an error.
3. Allocate a line ending at the first newline, or at the stash terminator.
4. Allocate the remaining suffix and free the previous stash.
5. Return the line; the caller must `free()` it.

```text
read:       first\nsecond\n
return:     first\n
retain:            second\n
```

The final line is returned even if it lacks a newline. EOF and errors both
produce NULL, so the interface does not distinguish them. The example likewise
cannot infer whether a NULL result means EOF or an internal error.

## Algorithms, Memory, and Design Decisions

The stash is a dynamically allocated C string, not a linked list. A single
static pointer makes repeated calls simple but **does not support interleaving
different descriptors**. It is also not reentrant or thread-safe.

The private `ft_strjoin` frees its first argument while constructing the joined
string. Its ownership differs from the libft function with the same name.
Repeated concatenation copies accumulated data; long-line time can therefore
be quadratic rather than linear. Working memory scales with the retained line
and read buffer, with temporary copies during concatenation/extraction.

## Verification and Limitations

On Fedora, 2026-10-27:

- Strict compilation, archive construction, and example linking passed.
- **21 byte-for-byte roundtrip cases** passed: seven text inputs at buffer
  sizes 1, 42, and 4096, including empty input, blank lines, a 10,000-character
  line, and a final unterminated line.
- A separate Clang ASan/UBSan contract check passed for returned line boundaries,
  caller frees, repeated EOF, a negative descriptor, and a closed descriptor.

The following source limitations remain: early invalid-descriptor returns do
not clear an existing stash; there is no explicit cleanup API if a caller stops
early; suffix-allocation failure can discard unread bytes after a line has been
extracted. Allocation-failure injection, concurrent use, interleaved descriptors,
and arbitrary binary data are not certified by the checks.

## Resources

- Linux manual pages: `read(2)`, `open(2)`, `close(2)`, `malloc(3)`.
- [Static storage duration](https://en.cppreference.com/w/c/language/storage_duration).
- [Original learning video](https://www.youtube.com/watch?v=8E9siq7apUU&t=1207s), recorded in the previous README; this implementation uses strings rather than its linked-list approach.
- Read/stash explanation structure informed by [oweidat05/Get-Next-Line](https://github.com/oweidat05/Get-Next-Line), without borrowing its complexity claims.

## AI Usage

The original README records AI help with documentation, planning, explanations
of descriptors/static state, and debugging. This publication used an AI
assistant to inspect the source, add build/example support, and run verification.
The line-reading implementation was preserved.
