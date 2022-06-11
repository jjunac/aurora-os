# Troubleshooting

## Running the kernel

### Boot

#### No bootable device found

If `grub-file --is-x86-multiboot` is successful but you get `No bootable device found`, this probably means that
`grub-mkrescue` generated a malformed image and silently failed (which is common). Check the size of the image, if it's
less than several MB, then that's it. Make sure that you've installed the following package:
```shell
sudo apt-get install -y grub-pc-bin
```

### Bochs

#### PANIC Message: dlopen failed for module 'x' (libbx_x.so): file not found

```shell
sudo apt-get install -y bochs* vgabios bochsbios
```

