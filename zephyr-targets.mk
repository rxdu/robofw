## Firmware App Targets
_clean_build:
	rm -rf build/$(folder)
	mkdir -p build

_conan_install:
	mkdir build/$(folder)
	conan install --install-folder=build/$(folder) ./conanfile.cm4 --remote=gitlab

_native_posix_conan_install:
	mkdir build/$(folder)
	conan install --install-folder=build/$(folder)/native_posix ./conanfile.i386 --remote=gitlab

_west_build:
	west build -d build/$(folder) -- -DBUILD_POSIX_NATIVE=OFF -DBOARD=$(board) -DAPP_NAME=$(app) \
		-DCONFIG_BOOTLOADER_MCUBOOT=y

_native_posix_build:
	west build -d build/$(folder)/native_posix -- -DBUILD_POSIX_NATIVE=ON

_build: _clean_build _west_build

_build_native: _clean_build _native_posix_conan_install _native_posix_build -DAPP_NAME=$(app)

## MCUBoot Targets
_build_mcuboot:
	rm -rf build/mcuboot
	rm -f mcuboot.conf
	./scripts/generate_mcuboot_conf.sh
	west build -d build/mcuboot -b $(board) -s $(ZEPHYR_BASE)/../bootloader/mcuboot/boot/zephyr/ -- \
		-DOVERLAY_CONFIG=$(shell pwd)/mcuboot.conf -DBOARD_ROOT=$(shell pwd) -DDTS_ROOT=$(shell pwd) \
		-DCONFIG_BOOTLOADER_MCUBOOT=y

## Sign Image
ifndef version
override version = 0.0.0
endif

_sign:
	west sign -t imgtool -p $(ZEPHYR_BASE)/../bootloader/mcuboot/scripts/imgtool.py -d build/$(folder) -- \
		--pad --version $(version) --key ./robofw.pem

## Flash signed image
_flash_signed_image:
	west -v flash -d build/$(folder) --hex-file build/$(folder)/zephyr/zephyr.signed.hex

## Upload signed image
ifndef dev
override dev = /dev/ttyUSB0
endif

ifndef ip_address
override ip_address = 10.10.0.10
endif

_upload_signed_image_serial:
	sudo mcumgr --conntype serial --connstring "$(dev),baud=115200" image upload build/$(folder)/zephyr/zephyr.signed.bin

_upload_signed_image_udp:
	sudo mcumgr --conntype udp --connstring=[$(ip_address)]:1337 image upload build/$(folder)/zephyr/zephyr.signed.bin
