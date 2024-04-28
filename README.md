# tinyfan

Originally for an x131e with an obnoxious fan; it works unmodified on a t440p, too.

To enable direct control of the fan, you need to pass options to the kernel driver (which will require a reboot to take effect)

```bash
sudo tee /etc/modprobe.d/thinkpad_acpi.conf < EOF
options thinkpad_acpi fan_control=1
EOF
```

`tinyfan` requires root privileges to run. It defaults to a (probably too high) 70C threshold to turn the fan on. It will extend the life of your machine to set this to a lower figure. In fact: you probably shouldn't run this at all, tbh. What are you even doing? At 80C it panics and blasts the fan at 100% hoping to correct the ship before we melt.
