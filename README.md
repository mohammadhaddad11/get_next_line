*This project was created as part of the 42 curriculum by mhaddad.*

# get_next_line

Read a file descriptor one line at a time, preserving any unread bytes across calls.

## Description

`char *get_next_line(int fd)` returns a newly allocated line, including `\n` when present. At end-of-file or on error it returns `NULL`. The caller must free each returned line.

## Instructions

Requires a C compiler, Make, and `ar` for the archive target.

```sh
git clone git@github.com:mohammadhaddad11/get_next_line.git
cd get_next_line
make
make example
./gnl_example README.md
```

The Makefile builds `libgnl.a`; `make example` compiles [a small file-reading driver](examples/read_lines.c). Use `make clean`, `make fclean`, and `make re` as needed.

You can also compile the original implementation directly:

```sh
cc -Wall -Wextra -Werror -DBUFFER_SIZE=42 -I. \
  examples/read_lines.c get_next_line.c get_next_line_utils.c -o gnl_example
```

## How It Works

1. Read up to `BUFFER_SIZE` bytes into a temporary buffer when the stash lacks a complete line.
2. Append the data to a **static stash** until a newline or EOF is found.
3. Allocate and return the next line.
4. Keep the remaining bytes for the following call.

```text
stash before extraction: "first\nsecond\n"
returned line:           "first\n"
stash after extraction:  "second\n"
```

The stash is a dynamically allocated string; repeated concatenation may copy data many times for long lines. The single static stash means **interleaved reads from multiple file descriptors are not supported**. The function is not reentrant or thread-safe. Its private `ft_strjoin` frees its first argument, unlike the standalone libft helper with the same name.

## Testing and Limitations

The published example and earlier targeted checks exercised empty input, blank lines, long lines, missing final newline, and several buffer sizes. The interface does not distinguish EOF from read/allocation errors because both return `NULL`; neither can the example driver infer that distinction.

Stopping before EOF can leave buffered data in the static stash; the implementation exposes no independent cleanup function. Early invalid-descriptor returns do not clear an existing stash. Allocation failure may also discard unread suffix data.

## Resources

- Manual pages: `read(2)`, `open(2)`, `close(2)`, `malloc(3)`
- [C storage duration](https://en.cppreference.com/w/c/language/storage_duration)
- [File I/O learning video](https://www.youtube.com/watch?v=8E9siq7apUU&t=1207s)

## AI Usage

The earlier documentation records AI assistance with learning/planning, static state, descriptors, memory handling, debugging, and README preparation. AI also assisted the publication example driver and focused verification. This refresh changes documentation only; the original line-reading implementation remains unchanged.
