# configuration file generated by Bochs

# for gdb stub
gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0

# For gdb stub
plugin_ctrl: unmapped=1, biosdev=1, speaker=1, extfpuirq=1, iodebug=0

# for built-in debugger
# plugin_ctrl: unmapped=1, biosdev=1, speaker=1, extfpuirq=1, iodebug=1

config_interface: textconfig
display_library: x
memory: host=32, guest=32
romimage: file="/usr/local/share/bochs/BIOS-bochs-latest"
vgaromimage: file="/usr/local/share/bochs/VGABIOS-lgpl-latest"
boot: cdrom
floppy_bootsig_check: disabled=0
# no floppya
# no floppyb
ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14

# for eclipse/gdb
#ata0-master: type=cdrom, path=$ISO_LOCATION, status=inserted, biosdetect=auto, model="Generic 1234"
ata0-master: type=cdrom, path="boot.iso", status=inserted, biosdetect=auto, model="Generic 1234"

ata1: enabled=1, ioaddr1=0x170, ioaddr2=0x370, irq=15
ata2: enabled=0
ata3: enabled=0
parport1: enabled=1, file=""
parport2: enabled=0
com1: enabled=1, mode=file, dev="serial.log"
com2: enabled=0
com3: enabled=0
com4: enabled=0
usb_uhci: enabled=0
usb_ohci: enabled=0
usb_xhci: enabled=0
pci: enabled=1, chipset=i440fx
vga: extension=none, update_freq=25
cpu: count=1, ips=4000000, model=atom_n270, reset_on_triple_fault=1, cpuid_limit_winnt=0, ignore_bad_msrs=1
cpuid: family=6, model=0x03, stepping=3, mmx=1, apic=xapic, sep=1, aes=0, xsave=0, xsaveopt=0, movbe=0, smep=0, mwait=1
cpuid: vendor_string="GenuineIntel"
cpuid: brand_string="     Intel(R) Atom(TM) CPU  270   @ 1.60GHz    "

print_timestamps: enabled=0
debugger_log: -

# For GDB stub
magic_break: enabled=0
# For GDB stub
port_e9_hack: enabled=0

# For built-in debugger
# magic_break: enabled=1
# port_e9_hack: enabled=1

private_colormap: enabled=0
clock: sync=none, time0=local
# no cmosimage
ne2k: enabled=0
#pnic: enabled=0
sb16: enabled=0
es1370: enabled=0
# no loader
log: -
logprefix: %t%e%d
panic: action=ask
error: action=report
info: action=report
debug: action=ignore
keyboard_type: mf
keyboard_serial_delay: 250
keyboard_paste_delay: 100000
keyboard_mapping: enabled=0, map=
user_shortcut: keys=none
mouse: enabled=0, type=ps2, toggle=ctrl+mbutton