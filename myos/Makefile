# Entry point
# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
ENTRYPOINT	= 0x1000

# Offset of entry point in kernal file
# It depends on ENTRYPOINT
ENTRYOFFSET	=   0x400

# Programs, flags, etc.
ASM		= nasm
DASM	= ndisasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -m32 -c -fno-builtin -fno-stack-protector 
LDFLAGS		= -m elf_i386 -s -Ttext $(ENTRYPOINT)
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

# This Program
ORANGESBOOT	= boot/boot.bin boot/loader.bin
ORANGESKERNAL	= kernal/kernal.bin
OBJS		= kernal/kernal.o kernal/syscall.o kernal/sys_call.o kernal/start.o kernal/i8259.o kernal/protect.o kernal/global.o kernal/main.o kernal/clock.o \
				kernal/keyboard.o kernal/tty.o kernal/console.o  kernal/stdio.o kernal/shell.o kernal/process.o kernal/semaphore.o \
				kernal/ipc.o kernal/hd.o \
				fs/fs.o fs/open.o fs/rdwt.o fs/del.o \
				usr/user.o usr/uliba.o usr/ulib.o \
				lib/klib.o lib/kliba.o lib/string.o 
				
DASMOUTPUT	= kernal.bin.asm

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
everything : $(ORANGESBOOT) $(ORANGESKERNAL)

all : realclean everything

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)

realclean :
	rm -f $(OBJS) $(ORANGESBOOT) $(ORANGESKERNAL)

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNAL) > $(DASMOUTPUT)

# We assume that "a.img" exists in current folder
buildimg :
	dd if=boot/boot.bin of=os.img bs=512 count=1 conv=notrunc
	sudo mount -o loop os.img dir
	sudo cp -fv boot/loader.bin dir
	sudo cp -fv kernal/kernal.bin dir
	sudo umount dir

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc \
			boot/include/fat12.inc boot/include/define.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(ORANGESKERNAL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(ORANGESKERNAL) $(OBJS)

kernal/kernal.o : kernal/kernal.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernal/syscall.o : kernal/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernal/main.o: kernal/main.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/start.o : kernal/start.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/sys_call.o : kernal/sys_call.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/clock.o: kernal/clock.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/i8259.o : kernal/i8259.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/global.o : kernal/global.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/protect.o : kernal/protect.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/keyboard.o : kernal/keyboard.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/tty.o :kernal/tty.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/console.o :kernal/console.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/stdio.o :kernal/stdio.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/shell.o :kernal/shell.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/process.o :kernal/process.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/semaphore.o :kernal/semaphore.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/ipc.o :kernal/ipc.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

kernal/hd.o :kernal/hd.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

fs/fs.o :fs/fs.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

fs/open.o :fs/open.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

fs/rdwt.o :fs/rdwt.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

fs/del.o :fs/del.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<

usr/user.o :usr/user.c 
	$(CC) $(CFLAGS) -o $@ $<

usr/uliba.o : usr/uliba.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

usr/ulib.o : usr/ulib.c
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.c include/all.h \
			include/keyboard.h include/type.h include/const.h include/protect.h \
			include/process.h include/proto.h include/global.h include/fs.h \
			include/console.h include/tty.h  include/semaphore.h include/hd.h include/ipc.h
	$(CC) $(CFLAGS) -o $@ $<
	
lib/kliba.o : lib/kliba.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<

