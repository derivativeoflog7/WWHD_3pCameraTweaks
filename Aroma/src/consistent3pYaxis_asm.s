.global consistent3pYaxis_return_call # Return to the replaced function, which in this case executes the instruction that was replaced by the jump, and ultimately jumps back to the game's code

.global consistent3pYaxis_custom_code
consistent3pYaxis_custom_code:
   # Game has just loaded the right stick Y position in f12 at this point
   lfs %f7, +0x10(%r1)     # Load target angle when zooming out (f7 is safe to use, as the game code overrides it soon after anyway)
   # Target angle when zooming in seems to always be in f26 at this point
   fcmpu %cr0, %f7, %f26   # Zoom out target angle < zoom in target angle?
   bge skip                # If true, negate the right stick Y position, otherwise do nothing
   fneg %f12, %f12
skip:
   # Build and execute jump to the replaced instruction and ultimate jump back to the game's code
   lis     %r11, consistent3pYaxis_return_call@ha
   lwz     %r11, consistent3pYaxis_return_call@l(%r11)
   mtctr   %r11
   bctr

# Unlike Cemu, we can't blr back to the original game code here, as FunctionPatcher uses ba to jump to custom code; thus using blr would jump who knows where and freeze/crash the game
