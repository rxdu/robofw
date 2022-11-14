############################################################
## general building procedures
############################################################

## Common zephyr build targets
include zephyr-targets.mk

# remove everything in build
.PHONY: clean
clean:
	rm -rf build

# bootloader targets
bl_tbotctrl_f405:
	${MAKE} _build_mcuboot board=tbotctrl_f405

bl_rcctrl_f405:
	${MAKE} _build_mcuboot board=rcctrl_f405

# flash bootloader
flash_bootloader: build/mcuboot
	west -v flash -d build/mcuboot

# firmware targets
fw_tbotctrl_f405:
	${MAKE} _build app=tbot board=tbotctrl_f405 folder=tbotctrl_f405

fw_rcctrl_f405:
	${MAKE} _build app=rccar board=rcctrl_f405 folder=rcctrl_f405

# sign and flash fw
flash_tbotctrl_f405: fw_tbotctrl_f405
	${MAKE} _sign folder=tbotctrl_f405
	${MAKE} _flash_signed_image folder=tbotctrl_f405

flash_rcctrl_f405: fw_rcctrl_f405
	${MAKE} _sign folder=rcctrl_f405
	${MAKE} _flash_signed_image folder=rcctrl_f405

# upload signed image
upload_tbotctrl_f405_serial: fw_tbotctrl_f405
	${MAKE} _sign folder=tbotctrl_f405
	${MAKE} _upload_signed_image_serial folder=tbotctrl_f405
