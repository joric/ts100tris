ts100tris
=========

This branch adds to Tetris accelerometer controls. Download precompiled firmware [here](https://raw.githubusercontent.com/joric/ts100tris/accelerometer/S100App/Exe/APP.hex).

Accelerometer
-------------

Stock 2.17 firmware has a bug with accelerometer (it only returns X values). Here's a fix:


```
@@ -170,8 +185,8 @@
                 gY_value.Byte.lo = value[3];
                 gZ_value.Byte.hi = value[4];
                 gZ_value.Byte.lo = value[5];
-                return 1;
             }
+            return 1;
         } else
             return 0;
     }
```

Mind that Update_X/Y/Z return unsigned values, so you also need a sign to determine left or right tilt:

```
int GetTilt_Y(void)
{
    return gY_value.Byte.hi>0x7f ? -1 : 1;
}
```



