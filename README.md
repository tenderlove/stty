# stty

* https://github.com/tenderlove/stty

## DESCRIPTION:

This is a minimal termios wrapper.  It's just enough C code to expose termios
functions and constants, and nothing more.  This is just a fun project I did
because I had a hard time using other serial port gems.

This is tested against my GMC-320 Geiger counter and I need to figure out how
to write real tests.

## Development

First compile:

```
$ git clone https://github.com/tenderlove/stty.git
$ cd stty
$ rake compile
```

Then hack.

## FEATURES/PROBLEMS:

* No tests yet (please submit them)

## SYNOPSIS:

This code gets the version for my GMC-320 Geiger counter.

```ruby
require 'stty'

Stty.open("/dev/tty.wchusbserial1420", 115200, "8N1") do |f|
  f.flush_input
  loop do
    f.write "<GETVER>>"
    x = f.read(14)
    if x
      p x
      break
    end
    puts "retrying"
    f.flush_input
  end
end
```

## LICENSE:

(The MIT License)

Copyright (c) 2016 Aaron Patterson

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
