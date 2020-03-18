# lolin_d32_waveshare

Create a `configuration.h` file with your WIFI credentials and timezone offset

    const char *ssid = "ssid";
    const char *password = "password";
    const long GmtOffsetSec = -3 * 3600;
    const int DaylightOffsetSec = 0;

## Libraries

* https://github.com/ZinggJM/GxEPD2

## Components

* [Lolin D32](https://wiki.wemos.cc/products:d32:d32)
* [Waveshare, 1.54inch E-Ink Display 200x200 Pixel 3.3V/5V Support Partial Refresh](https://www.amazon.com/gp/product/B07VD1VMMH/)

## References

* https://github.com/G6EJD/LiPo_Battery_Capacity_Estimator
