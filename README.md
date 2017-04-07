# Prime Map

A simple multi-threaded program that finds primes and sores them in a cool looking bitmap

## Installation

Please use `<make> .\bin\make.exe` to compile the program. You can also use `<make> all`.

## Usage

Run the excutable with these parameters:

| Parameter | Default |                                                                          |
|-----------|---------|--------------------------------------------------------------------------|
| -t        | 4       | Use this argument to specify number of threads                           |
| -m        | 10000   | Use this argument to specify the maximum number                          |
| -o        | False   | Use this switch to specify whether to output a bitmap                    |
| -d        | False   | Use this switch to specify whether to display the bitmap                 |
| -gbc      |         | Use this switch to use green on black color scheme                       |
| -p        | 000000  | Use this argument to specify prime color on the bitmap (HEX format)      |
| -b        | FFFFFF  | Use this argument to specify background color on the bitmap (HEX format) |

Usage:   .\\PrimeMap.exe [-t <threads>] [-e <end>] [-o] [-d] [[-gbc] | [-p <HEX CODE>] [-b <HEX CODE>]]

Example: .\\PrimeMap.exe -t 7 -e 500000 -o -d -gbc

Example: .\\PrimeMap.exe -t 7 -e 500000 -o -p 010101 -d 245 12 155
  
## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## License

Copyright (c) 2017 The Gupta Empire - All Rights Reserved

Copyright (c) 2017 Aryan Gupta
