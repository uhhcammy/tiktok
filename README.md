# Tiktok

Tiktok is a tiny clock app for your desktop!

  - Make it look pretty!
  - Simple and easy to use!
  - Sparkles

## Features

  - Use your favorite fonts!
  - Color picker to quickly change the appearance
  - More in the future..?

## Instalation

Tiktok requires GTK+ 2.

To build it just type:

```sh
$ make
```

You can also compile it like this:

```sh
gcc main.c -o tiktok $(pkg-config --cflags --libs gtk+-2.0)
```

## Application options

Use them to change how it looks!

|Full|Short|Description|
|---|---|---|
|--format|-d|Sets date format.<br>Full list of tokens available [here](http://man7.org/linux/man-pages/man3/strftime.3.html)|
|--font|-f|Sets font family.<br>You can use any TrueType fonts just place them in .fonts folder in your home directory. |
|--color|-c|Sets text color.<br>It can be a hex value in the form '#rgb' '#rrggbb' or [color name](https://en.wikipedia.org/wiki/X11_color_names#Color_name_chart).|
|--background|-b|Sets background color.<br>It can be a hex value in the form '#rgb' '#rrggbb' or [color name](https://en.wikipedia.org/wiki/X11_color_names#Color_name_chart).|
|--size|-s|Sets font size in pixels.|
|--outline|-o|Sets outline color.<br>It can be a hex value in the form '#rgb' '#rrggbb' or [color name](https://en.wikipedia.org/wiki/X11_color_names#Color_name_chart).|
|--thickness|-t|Sets outline thickness in pixels|
|--x|-x|Sets horizontal position.<br>You can use negative value to start from the right edge of the screen.|
|--y|-y|Sets outline thickness in pixels|
|--x|-x|Sets vertical position.<br>You can use negative value to start from the bottom edge of the screen.|
|--gravity|-g|Sets which point of the window will be placed at x, y coordinates.<br>1 = top left corner<br>2 = middle of the top edge<br>3 = top right corner<br>5 = center of the window<br>7 = lower left corner<br>8 = middle of the lower edge<br>9 = lower right corner |

## Done!
Thanks for trying out my app! I hope you have a wonderful day!!
:hearts:
