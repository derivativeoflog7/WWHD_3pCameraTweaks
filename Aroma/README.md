## Usage
The various patches can be enabled/disabled via the WUPS Config menu (press L, DPAD Down and Minus on the GamePad, Pro Controller or Classic Controller).  

## Installation

(`[ENVIRONMENT]` is a placeholder for the actual environment name.)

1. Copy the file `WWHD_3pCameraTweaks.wps` into `sd:/wiiu/environments/[ENVIRONMENT]/plugins`.
2. Requires the [WiiUPluginLoaderBackend](https://github.com/wiiu-env/WiiUPluginLoaderBackend) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.
3. Requires the [FunctionPatcherModule](https://github.com/wiiu-env/FunctionPatcherModule/) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.

**NOTE: both modules are already included in a standard [base Aroma](https://aroma.foryour.cafe/) install.**
Start the environment (e.g Aroma) and the backend should load the plugin.

## Building

For building you need:

- [wups](https://github.com/Maschell/WiiUPluginSystem)
- [wut](https://github.com/devkitpro/wut)
- [libkernel](https://github.com/wiiu-env/libkernel)
- [libfunctionpatcher](https://github.dev/wiiu-env/libfunctionpatcher)

Install them (in this order) according to their README's. Don't forget the dependencies of the libs itself.

Then you should be able to compile via `make` (with no logging), `make DEBUG=1` (with logging) or `make DEBUG=VERBOSE` (with verbose logging).

### Logging

Building via `make` only logs errors (via OSReport). To enable logging via the [LoggingModule](https://github.com/wiiu-env/LoggingModule) set `DEBUG` to `1` or `VERBOSE`.

`make` Logs errors only (via OSReport).  
`make DEBUG=1` Enables information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).  
`make DEBUG=VERBOSE` Enables verbose information and error logging via [LoggingModule](https://github.com/wiiu-env/LoggingModule).

If the [LoggingModule](https://github.com/wiiu-env/LoggingModule) is not present, it'll fallback to UDP (Port 4405) and [CafeOS](https://github.com/wiiu-env/USBSerialLoggingModule) logging.

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t wwhd_3pcameratweaks-builder

# make 
docker run -it --rm -v ${PWD}:/project WWHD_3pCameraTweaks-builder make

# make clean
docker run -it --rm -v ${PWD}:/project WWHD_3pCameraTweaks-builder make clean
```
