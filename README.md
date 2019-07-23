# HTQ

HTQ is a grep-like tool for extracting elements from html streams using CSS3 selectors. It uses the incomparable [libmodest](https://github.com/lexborisov/Modest) for html and css parsing and css selector filtering.

## Building

```shell
git submodule init
git submodule update
make
```

## Usage

```
usage: htq [css_query] [options] [file ...]
    -c QUERY, --css=QUERY            Specify a css selector
    -a ATTR, --attr=ATTR             Extract an attribute value
    -p, --pretty                     Pretty print output
    -t, --text                       Print text content
    -0, --print0                     Separate output by NULL
    -l, --list-files                 List matching files without matches
    -h, --help                       Print help message
```

## Examples

```html
<!-- test.html -->
<!doctype html>
<html>
  <body>
    <h1>Test Page</h1>
    <blockquote>
      <p>Oh how I wish I could shut up like a telescope!</p>
      <p>I think I could, if I only knew how to begin.</p>
      <footer> — Alice in Wonderland</footer>
    </blockquote>
    <ul>
      <a href="#link-1">Link 1</a>
      <a href="#link-2">Link 2</a>
    </ul>
  </body>
</html>
```

```shell
$ htq p test.html
<p>Oh how I wish I could shut up like a telescope!</p>
<p>I think I could, if I only knew how to begin.</p>
$ htq p --text test.html
Oh how I wish I could shut up like a telescope!
I think I could, if I only knew how to begin.
$ htq a --attr href test.html
#link-1
#link-2
$ curl -s https://example.com | htq h1
<h1>Example Domain</h1>
```
