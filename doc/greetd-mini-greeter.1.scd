greetd-mini-greeter(1)

# NAME

greetd-mini-greeter - An extremely simple GTK3/4 greeter for greetd

# SYNOPSIS

*greetd-mini-greeter* -u user -c command [-s file]

# DESCRIPTION

greetd-mini-greeter is a GTK3/4 greeter for greetd, designed for a single user.

greetd-mini-greeter requires a Wayland compositor to run, such as *sway*(1).

# OPTIONS

*-u, --user*
	The user to login as.

*-c, --command*
	The command to run on login.

*-s, --style*
	The css file to use for styling.

# FILES

Note: these are the defaults as defined at compile time.

_CSS_PATH_
	The default css file used for styling.

# EXAMPLES

When using sway as the compositor for greetd-mini-greeter, something like the
following sway config lines produce a nice look:

```
# Remove any window borders
default_border none
hide_edge_borders both

# Add a background image
output * background /path/to/image fill
```

# AUTHORS

Philip Jones <philj56@gmail.com>

# SEE ALSO

*greetd*(1)
