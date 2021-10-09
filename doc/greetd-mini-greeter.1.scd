greetd-mini-greeter(1)

# NAME

greetd-mini-greeter - An extremely simple GTK4 greeter for greetd

# SYNOPSIS

*greetd-mini-greeter* -u user -c command [-s file]

# DESCRIPTION

greetd-mini-greeter is a GTK4 greeter for greetd, designed for a single user.

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
default_border none
hide_edge_borders both

# If using a background image for greetd-mini-greeter,
# to remove the initial flicker of grey
output * background /path/to/image fill
```

# AUTHORS

Philip Jones <philj56@gmail.com>

# SEE ALSO

*greetd*(1)
