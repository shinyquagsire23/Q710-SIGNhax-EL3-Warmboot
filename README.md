This code has *only* been tested on a Q710ULM, flashed to 20c_00_AMZ_US_OP_1121. Do not flash this to any other phone, or you *will* brick. Probably.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Usage Instructions:
- Run build.sh (optional; prebuilt images are included in the repository)
- Boot the phone into LAF by holding Volume Up with the data cable attached
- Run upload.sh. This will use the SIGN vulnerability to write the aboot 888rle payload.
- Hold Vol Up, Vol Down, and Power until the phone reboots and shows a white screen, then release.
- The phone will now execute the aboot 888rle vulnerability, which will craft a payload for the Demigod vulnerability
- The phone will present a gray screen and the Demigod crash handler momentarily before switching to a green screen, indicating that SBL1 code execution was achieved.
- The SBL1 payload will resume boot execution and patch aboot to allow unsigned `fastboot boot` images to be uploaded via USB.
