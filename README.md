### Radiant FontEdit (Unicode compatible version)

This is a bitmap font editor, originally created by Pieter Simoons (aka Radiant) around mid-2000ies for use along with the [Adventure Game Studio](https://www.adventuregamestudio.co.uk/). The editor supports loading and saving fonts as SCI (Sierra font format) and WFN (bitmap font format used by AGS). It also can import TTF font, and import/export whole font as a bitmap.

The editor in this repository has been modified:
- Project updated to MSVS 2019.
- Editor now supports editing and saving up to 64k glyphs in a single font, which lets create Unicode-compatible bitmap fonts for AGS.

### Building

Editor is based on MFC library, and thus is a strictly MS Windows application.
Requires MSVS 2019 or higher.

### License

[GNU Lesser General Public License 3.0](LICENSE.md)
