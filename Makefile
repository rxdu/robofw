############################################################
## general building procedures
############################################################

# remove everything in build
clean:
	rm -rf build

# usage: build_mcuboot <board=?>
build_mcuboot:
	rm -rf build/mcuboot
	rm -f mcuboot.conf
	./scripts/generate_mcuboot_conf.sh
	west build -d build/mcuboot -b $(board) -s $(ZEPHYR_BASE)/../bootloader/mcuboot/boot/zephyr/ -- -DOVERLAY_CONFIG=$(shell pwd)/mcuboot.conf -DBOARD_ROOT=$(shell pwd)

# usage: build_fw_mcu <robot=?>
build_fw_mcu: 
	west build -d build/$(robot) -- -DBUILD_POSIX_NATIVE=OFF -DROBOT=$(robot)

# usage: build_fw_np <robot=?>
build_fw_np:
	west build -d build/$(robot)/native_posix -- -DBUILD_POSIX_NATIVE=ON -DROBOT=$(robot)

# usage: sign_fw <robot=?> <version=?>
sign_fw: 
	west sign -t imgtool -p $(ZEPHYR_BASE)/../bootloader/mcuboot/scripts/imgtool.py -d build/$(robot) -- --pad --version $(version) --key ./robofw.pem

# usage: flash_fw <robot=?>
flash_fw:
	west flash -d build/$(robot) --hex-file build/$(robot)/zephyr/zephyr.hex

# usage: flash_signed_fw <robot=?>
flash_signed_fw:
	west flash -d build/$(robot) --hex-file build/$(robot)/zephyr/zephyr.signed.hex

############################################################

## Build bootloader for robots
mcuboot_all: mcuboot_tbot

mcuboot_tbot:
	${MAKE} build_mcuboot board=tbotctrl_f405

flash_mcuboot: build/mcuboot
	west flash -d build/mcuboot

## Build firmware for robots
fw_tbot_np: 
	${MAKE} build_fw_np robot=tbot

# image without bootloader
fw_tbot: 
	${MAKE} build_fw_mcu robot=tbot

flash_fw_tbot:
	${MAKE} flash_fw robot=tbot

# signed image for mcuboot
signed_fw_tbot: fw_tbot
	${MAKE} sign_fw robot=tbot

flash_signed_tbot:
	${MAKE} flash_signed_fw robot=tbot

############################################################
## Upload signed image with mcumgr
############################################################

ifndef dev
override dev = /dev/ttyUSB0
endif

ifndef ip_address
override ip_address = 10.10.0.10
endif

mcumgr_upload_serial:
	sudo mcumgr --conntype serial --connstring "$(dev),baud=115200" image upload build/$(robot)/zephyr/zephyr.signed.bin

mcumgr_upload_udp:
	sudo mcumgr --conntype udp --connstring=[$(ip_address)]:1337 image upload build/$(robot)/zephyr/zephyr.signed.bin

upload_tbot_serial: build/tbot
	${MAKE} signfw robot=tbot
	${MAKE} mcumgr_upload_serial robot=tbot

upload_tbot_udp: build/tbot
	${MAKE} signfw robot=tbot
	${MAKE} mcumgr_upload_udp robot=tbot
