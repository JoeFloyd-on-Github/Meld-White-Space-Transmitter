# FCC equipment authorization — FCC ID `OKVMT300`

The MT300 is an FCC-certified transmitter, and its certification record is public.
This page summarizes the grant and **mirrors the full set of public exhibits**
(manuals, photos, test reports, attestations, letters) into
[`fcc/exhibits/`](fcc/exhibits) so the regulatory record travels with this repo.

Mirrored 2026‑11‑22 from the FCC OET Equipment Authorization System (via the
public mirror `fccid.io/OKVMT300`). These are Meld Technology's own public filing
documents from 2012; nothing here is device‑owner data.

## The grant

| Field | Value |
|---|---|
| FCC ID | **OKVMT300** (grantee code `OKV`, product code `MT300`) |
| Grantee | **Meld Technology, Inc.**, Sunnyvale, CA |
| FCC model | `MT300v1` |
| Equipment class | **White Space Device with Geo‑location — Fixed** (`WGF`); "Low Power Fixed TVBD" |
| FCC rule part | **Part 15, Subpart H** (`15H`) — TV white space |
| Granted | **2012‑11‑28** (application dated 2012‑08‑20) |
| Frequency ranges | **515–593 MHz** and **623–695 MHz** |
| Output power | **0.014 W (14 mW), conducted** |
| Frequency tolerance | 50 ppm |
| Emission designator | `C1F` |
| Test firm | Timco Engineering, Inc. (Newberry, FL) |
| FCC filing agent | **Spectrum Bridge, Inc.** |
| Responsible party | Jordan Du Val, President/CEO, Meld Technology |

### Grant use conditions (verbatim)

> Output power is conducted. The device is approved with the following use
> conditions: 1. This device must be professionally installed by an installer
> authorized by the grantee and installed in a permanent manner, such as a locked
> enclosure or secure fastening, which prevents the device from being readily
> moved. 2. This device is only authorized to transmit on TV Channels 21‑34
> (512‑596 MHz) and Channels 39‑51 (620‑698 MHz). 3. This device is approved for
> operation with the Spectrum Bridge TVWS database. 4. The use of a directional
> antenna or antenna gain higher than 2 dBi is prohibited. 5. Users and installers
> must be provided with antenna installation instructions and transmitter
> operating conditions for satisfying RF exposure compliance. A minimum separation
> distance of 40 cm should be maintained between the device and any human body.

## What the grant tells us about this project

- **The cloud dependency is written into the FCC authorization itself.**
  Condition 3 — *"approved for operation with the Spectrum Bridge TVWS database"* —
  means the device's legal right to transmit was conditioned on the very service
  (`tvws.spectrumbridge.com`) that was later shut down, bricking every unit. This
  is the regulatory root of the problem this repo exists to work around (see
  [`../README.md`](../README.md) and [`wsa-overview.md`](wsa-overview.md)). It also
  means that restoring transmission with a local stand‑in reproduces the *answer*
  the database used to give, but does not reproduce the FCC‑contemplated channel
  **authority** — operating on air still requires following the grant conditions.
- **Authorized channels are 21‑34 and 39‑51, not a flat 21‑51.** The grant
  excludes channels 35‑38 (channel 37 is the radio‑astronomy guard band). The
  firmware/web‑UI offers a `21‑51` list and the agent filters within that, but the
  *FCC‑authorized* set is narrower — a distinction worth keeping in view alongside
  the 600 MHz‑repack notes in [`HOW-TO.md`](../HOW-TO.md) and
  [`hardware-versions.md`](hardware-versions.md). (Both grant ranges predate the
  repack; channels 38‑51 are now licensed mobile spectrum.)
- **Rated output is 14 mW conducted.** The sell sheet/manual's "up to 40 mW" is
  the adjustable/EIRP figure; the FCC grant certifies **0.014 W conducted** into
  the ≤2 dBi antenna. Professional installation and a 40 cm separation distance are
  grant conditions.
- **Geo‑location is "Fixed, professional‑installer determined"** — the unit is a
  *fixed* TVBD whose location is set at install, which is why the web‑UI
  registration asks for latitude/longitude by hand rather than using a GPS.

## Hardware, from the internal photos

The internal‑photo exhibits show the original MT300 mainboard, silk‑screened
**`PCB-1814`** (the certified 2012 board — the later AV units in this repo carry
the `MT350 PCB Rev C` spin, see [`hardware-versions.md`](hardware-versions.md)).
Visible on it: the central **ViXS XCode** SoC (the MIPS video/transport processor
this project reverse‑engineers), a **Micrel** Ethernet device, a shielded RF/
modulator section, an **SD‑card** slot, and the rear I/O — composite/component RCA
jacks, **USB**, a **DB9 serial console**, **RJ45**, and an **SMA** antenna jack.
See [`fcc/exhibits/photo_internal_overview.jpg`](fcc/exhibits/photo_internal_overview.jpg)
and the top/bottom views.

## Mirrored exhibits

All under [`fcc/exhibits/`](fcc/exhibits) (originals as filed; PDFs except the
photos, which the source serves as JPEG):

| Exhibit | Type | File |
|---|---|---|
| MT300 Operator Manual V1.3 | User manual | [`manual_MT300-Operator-Manual-V1.3.pdf`](fcc/exhibits/manual_MT300-Operator-Manual-V1.3.pdf) |
| MT300 Operator Manual V1.4 | User manual | [`manual_MT300-Operator-Manual-V1.4.pdf`](fcc/exhibits/manual_MT300-Operator-Manual-V1.4.pdf) |
| Operator Manual | User manual | [`manual_Operator-Manual.pdf`](fcc/exhibits/manual_Operator-Manual.pdf) |
| Revised Operator Manual | User manual | [`manual_Revised-Operator-Manual.pdf`](fcc/exhibits/manual_Revised-Operator-Manual.pdf) |
| Internal photo — overview | Internal photos | [`photo_internal_overview.jpg`](fcc/exhibits/photo_internal_overview.jpg) |
| Internal photo — top | Internal photos | [`photo_internal_top.jpg`](fcc/exhibits/photo_internal_top.jpg) |
| Internal photo — bottom | Internal photos | [`photo_internal_bottom.jpg`](fcc/exhibits/photo_internal_bottom.jpg) |
| External photo — front | External photos | [`photo_external_front.jpg`](fcc/exhibits/photo_external_front.jpg) |
| External photo — rear | External photos | [`photo_external_rear.jpg`](fcc/exhibits/photo_external_rear.jpg) |
| External photo — top | External photos | [`photo_external_top.jpg`](fcc/exhibits/photo_external_top.jpg) |
| External photo — bottom | External photos | [`photo_external_bottom.jpg`](fcc/exhibits/photo_external_bottom.jpg) |
| Labels and Placement | ID label / location | [`label_Labels-and-Placement.pdf`](fcc/exhibits/label_Labels-and-Placement.pdf) |
| Antenna Data Sheet | Operational description | [`operational_Antenna-Data-Sheet.pdf`](fcc/exhibits/operational_Antenna-Data-Sheet.pdf) |
| MT300 ID signal | Operational description | [`operational_MT300-ID-signal.pdf`](fcc/exhibits/operational_MT300-ID-signal.pdf) |
| Test Setup Photos | Test setup photos | [`test-setup-photos.pdf`](fcc/exhibits/test-setup-photos.pdf) |
| Database Interface Certification Tests | Test report | [`test-report_Database-Interface-Certification-Tests.pdf`](fcc/exhibits/test-report_Database-Interface-Certification-Tests.pdf) |
| Radio Frequency Certification Tests | Test report | [`test-report_Radio-Frequency-Certification-Tests.pdf`](fcc/exhibits/test-report_Radio-Frequency-Certification-Tests.pdf) |
| Revised RF Certification Tests | Test report | [`test-report_Revised-RF-Certification-Tests.pdf`](fcc/exhibits/test-report_Revised-RF-Certification-Tests.pdf) |
| Meld Agency Agreement | Attestation | [`attestation_Meld-Agency-Agreement.pdf`](fcc/exhibits/attestation_Meld-Agency-Agreement.pdf) |
| Restricted Channel Operation | Attestation | [`attestation_Restricted-Channel-Operation.pdf`](fcc/exhibits/attestation_Restricted-Channel-Operation.pdf) |
| Confidentiality Request | Cover letter | [`letter_Confidentiality-Request.pdf`](fcc/exhibits/letter_Confidentiality-Request.pdf) |
| Confidentiality Request (revised) | Cover letter | [`letter_Confidentiality-Request-Revised.pdf`](fcc/exhibits/letter_Confidentiality-Request-Revised.pdf) |
| Meld Cover Letter | Cover letter | [`letter_Meld-Cover-Letter.pdf`](fcc/exhibits/letter_Meld-Cover-Letter.pdf) |
| MPE Analysis | RF exposure | [`rf-exposure_MPE-Analysis.pdf`](fcc/exhibits/rf-exposure_MPE-Analysis.pdf) |

The **Database Interface Certification Tests** report (Timco Engineering, Part 15
Subpart H Part 2) is the most directly relevant to this project — it is the FCC
test of the device's conversation with the Spectrum Bridge database, the same
exchange reproduced in [`wsa-overview.md`](wsa-overview.md) and served locally by
`sb_mips`. Schematics, block diagrams and bills of material were filed under
long‑term confidentiality and are **not** part of the public record, so they are
not here.
