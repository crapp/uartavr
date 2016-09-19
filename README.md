[![uartavr License](https://img.shields.io/badge/license-BSD--3--Clause-blue.svg)](#license)
[![uartavr latest release](https://img.shields.io/github/release/crapp/uartavr.svg)](https://github.com/crapp/uartavr/release/latest)

# uartavr

Simple interrupt driven uart library for AVR microcontrollers. This library uses
a circular buffer data structure to store data that was received or will be sent.
This way all write or read calls are nonblocking.

## Usage

Add the header and source file to your project.

### API Documentation

Have a look at the API documentation hosted by github

### Example

Minimum Code example for sending data with the UART

```c
/* create a cfg struct instance */
struct UARTcfg cfg;
memset(&cfg, 0, sizeof(struct UARTcfg));

/* init the config with default values */
init_uart_cfg(&cfg);
/* init the device UART using our configuration */
initUART(&cfg);

/* activate interrupts */
sei()

char *my_string = "This is your trusty micro";
/* send a string. The data is written into the circular buffer. It will be
transmitted using a ISR */
puts_UART(my_string);

```
## Development

The most important facts of the uartavr development process are explained here

### Branches

The github repository of uartavr has several different branches

* master: Main development branch. Everything in here is guaranteed to compile
and is tested (at least a little bit :)). This is the place for new features
and bugfixes. Pull requests welcome.
* dev: Test branch. Code is guaranteed to compile on the developers build
environment. Nothing more nothing less.
* release-x.x: Branch for a release. Only bugfixes are allowed here.
Pull requests welcome.
* gh-pages: Special branch for the static API HTML documentation that will be
hosted by github.io. Content is generated with doxygen.

### Coding standards

The source code is formatted with clang-format using the following configuration

```
Language                            : Cpp,
BasedOnStyle                        : LLVM,
AccessModifierOffset                : -4,
AllowShortIfStatementsOnASingleLine : false,
AlwaysBreakTemplateDeclarations     : true,
ColumnLimit                         : 81,
IndentCaseLabels                    : false,
IndentWidth                         : 4,
TabWidth                            : 4,
BreakBeforeBraces                   : Linux,
CommentPragmas                      : '(^ IWYU pragma : )|(^.*\[.*\]\(.*\).*$)|(^.*@brief|@param|@return|@throw.*$)|(/\*\*<.*\*/)'
```

### Versioning

I decided to use [semantic versioning](http://semver.org/) and stick to their rules.

> Given a version number MAJOR.MINOR.PATCH, increment the:
>
> 1. MAJOR version when you make incompatible API changes,
>
> 2. MINOR version when you add functionality in a backwards-compatible manner, and
>
> 3. PATCH version when you make backwards-compatible bug fixes.

We are currently at this stage

> Major version zero (0.y.z) is for initial development. Anything may change at
> any time. The public API should not be considered stable.

## Status

This library has not reached a stable version yet. Meaning the public API and
other things may be subjected to changes at any time. There have been no code
reviews so far and there are no unit tests. In spite of all that the library is
absolutely usable and works great for what it was developed for.

## Bugs, feature requests, ideas

Please use the [github bugtracker](https://github.com/crapp/uartavr/issues)
to submit bugs or feature requests

## FAQ

**Yet another uart lib, why?**

Most implementations use a copyleft license which I didn't want. Additionally
this library is easy to use and offers all options I need.


**Why does it not have feature X?**

Feel free to fork the project and make a pull request!

## License

```
uartavr interrupt driven serial communication for 8bit avrs
Copyright © 2016 Christian Rapp
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the organization nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ''AS IS'' AND ANY  EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL yourname BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
