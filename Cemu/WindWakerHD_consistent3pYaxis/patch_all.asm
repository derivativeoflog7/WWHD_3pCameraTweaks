[WindWakerHD_consistent3pYaxis]
moduleMatches = 0x74bd3f6a,0x475bd29f,0xb7e748de

.origin = codecave

# Note for all regions: it is safe to use bl to call this custom code, as all code paths after the replaced instruction will call a function with bl anyway
consistent_Y_fix:
lfs f12, +0x174(r31) # Load right stick Y position (original instruction that was replaced)
lfs f7, +0x10(r1) # Load target angle when zooming out (f7 is safe to use, as the game code overrides it soon after anyway)
# Target angle when zooming in seems to always be in f26 at this point
fcmpu cr0, f7, f26 # Zoom out target angle < zoom in target angle?
bge skip # If true, negate the right stick Y position, otherwise do nothing
fneg f12, f12
skip:
blr # Return to game code
