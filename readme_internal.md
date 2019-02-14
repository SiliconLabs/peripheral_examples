# MCU Peripheral Examples #

This repo contains simple peripheral examples based on emlib.

Owner: Stephen To

## How to create a new example for this repo ##
Prerequisites

- Simplicity Studio v4, GCC, and MCU SDK v5.1.2 or later
- IAR Embedded Workbench
- Python 2.7
- mcu\_peripheral\_examples (this) repo
- [s024_kits](https://stash.silabs.com/projects/MCU32_REPOS/repos/s024_kits/browse) repo
- [s026_utils](https://stash.silabs.com/projects/MCU32_REPOS/repos/s026_utils/browse) repo
- [32-bit coding standard](https://stash.silabs.com/projects/WMN_DOCS/repos/docs_misc/browse/software/standards/coding-standard.md)

Procedure

1. Copy template into new folder
2. Edit `project.xml`
3. Run `genproject.py` (from s026_utils) to generate IDE project files
4. Import project into IDE (Project -> Import -> MCU Project)
5. Edit source file(s)
6. Repeat steps 2-5 as necessary

To add support for a new kit to an existing project, follow steps 2-6 above.

## Known issues ##
1. `genproject.py` does not detect BRD name correctly from the kit config variable for wireless radio boards. Solution: use latest genproject.py on the master branch.

2. Build fails due to missing compiler symbol for the MCU after importing a slsproj file. Solution: update `AllPartsModule.slproj` from the [studio\_metadata repo](https://stash.silabs.com/projects/EMBSW/repos/studio_metadata/browse/modules), then copy to your `C:\SiliconLabs\SimplicityStudio\v#\developer\sdks\gecko\_sdk\_suite\v#.#\.studio\modules`

3. Build fails due to missing include path for kitconfig header files after importing a slsproj file. Solution: update `AllPartsModule.slproj` from the [studio\_metadata repo](https://stash.silabs.com/projects/EMBSW/repos/studio_metadata/browse/modules), then copy to your `C:\SiliconLabs\SimplicityStudio\v#\developer\sdks\gecko\sdk\suite\v#.#\.studio\modules`

4. Build fails due to missing definitions for `BSP_GPIO_LED0_PORT`, `BSP_GPIO_LED0_PIN`, `BSP_GPIO_LED1_PORT`, `BSP_GPIO_LED1_PIN`. Solution: update `bspconfig.h` for the kit, found in the [S024_kits repo](https://stash.silabs.com/projects/MCU32_REPOS/repos/s024_kits/browse). Then copy top level folders from this repo to `C:\SiliconLabs\SimplicityStudio\v#\developer\sdks\gecko\sdk\suite\v#.#\hardware\kit`

5. `genproject.py` does not include the `C:\SiliconLabs\SimplicityStudio\v#\developer\sdks\gecko_sdk_suite\v#.#\hardware\kit\<board>\config` folder for some of the series 1 radio boards (we believe it's mainly for the Nerio and Nixi radio boards). A hack for this is to edit the `project.xml` file, find the line that has `<kitconfig>` (should be under `directories`), make a copy of that line, paste it below, and finally change all occurrences of `kitconfig` to `kitconfig2` in that line. Then, find the next line that has <kitconfig> (should be under `includepaths`) and repeat the process. You can do this for every board in the `project.xml` that is missing the include specified above. Once the script is fixed, the best way to remove the hack would be to find lines that have `kitconfig2` in them and completely delete those lines. To make it easily scriptable, please use the arbitrary name `kitconfig2`.
