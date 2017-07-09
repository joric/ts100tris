ts100tris
=========

Autorotate branch

Autorotation
=============

I've used accelerometer code from the single button operation mod:

http://www.minidso.com/forum.php?mod=viewthread&tid=981

Simply overwrite MMA8652FC.* files with files from this mod.

You'd also have to copy `Get_MmaActive(void)` function from 2.17 because it's missing there.

It introduces a new API for getting axis direction, for example `Get_XYZDrt(AXIS_Y)` returns 1 with left tilt and 0 with right tilt, so you can implement autorotation.

Just like so (add this code to the beginning of the OLed_Display function):

```
Check_Accelerated();
gHanders = Get_XYZDrt(AXIS_Y);
if (device_info.handers != gHanders) {
  device_info.handers = gHanders;
  Init_Oled();
}
```

That's, basically, it.

There's still a small bug with config mode (pictures flipped incorrectly) but otherwise it works fine.


