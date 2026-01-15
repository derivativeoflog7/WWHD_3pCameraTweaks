FROM ghcr.io/wiiu-env/devkitppc:20250608

COPY --from=ghcr.io/wiiu-env/wiiupluginsystem:20260109 /artifacts $DEVKITPRO
COPY --from=ghcr.io/wiiu-env/libkernel:20260112 /artifacts $DEVKITPRO
COPY --from=ghcr.io/wiiu-env/libfunctionpatcher:20260112 /artifacts $DEVKITPRO

WORKDIR project
