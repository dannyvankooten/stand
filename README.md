# Stand!

Stand is a simple tool to remind you periodically to take a break, stand up from your desk and move around for a bit.

It depends on libnotify for its notifications.

![Screenshot of the Stand! notification](screenshot.png)

## Installation

Download a precompiled binary or compile from source:

```sh 
make
sudo make install 
```

## Usage 

Run `stand` without any arguments to use the default interval of `1 hour`.

You can provide a second argument to specify a custom interval, in seconds. 
For example, running `stand 1800` would remind you every 30 minutes.

## License

MIT licensed