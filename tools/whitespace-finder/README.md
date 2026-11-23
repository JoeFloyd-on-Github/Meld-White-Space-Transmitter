# tools/whitespace-finder — which TV channels are open for white space?

A small, dependency-free browser tool that reads the **latest FCC protected-entity
data** (mirrored in [`data/`](data)) and, for a location you enter, estimates which
TV channels may be open for **TV white space (TVWS)** use — the same kind of
decision the dead `tvws.spectrumbridge.com` database used to make for the Meld
MT300 that this repo revives.

> **Informational only — not an FCC white-space determination.** Legal
> transmission still requires a *certified* white-space database and device
> registration. This is a simplified, distance-based approximation of the FCC
> §15.712 protection rules, not the real F(50,90) contour math. For the MT300
> specifically, its FCC grant ([`../../docs/fcc.md`](../../docs/fcc.md)) authorizes
> only channels **21–34 and 39–51**.

## Run it

Browsers block `fetch()` of local files over `file://`, so serve the folder:

```bash
cd tools/whitespace-finder
python3 -m http.server 8000
```

Then open <http://localhost:8000/>. Enter a latitude/longitude, **pick a TV
market from the dropdown**, or hit "Use my location" — and it lists every channel
(defaulting to **All 2–51: VHF + UHF + 600 MHz**, with filters for **VHF only 2–13**,
**All TV 2–36**, **UHF only 14–36**, and **MT300 band 21–51**) with a status and the
nearest occupant. See also the sibling [`tools/radio-finder/`](../radio-finder/) tool
for AM & FM broadcast stations by market.

### Note on VHF channels after the DTV transition

Moving out of VHF was **not** mandatory in the 2009 US digital TV transition,
though many stations preferred UHF (14–51) because VHF has higher ambient/impulse
noise and requires larger receive antennas. Channels **2–13** remain active TV
broadcast channels (Low-VHF 2–6 at 54–88 MHz; High-VHF 7–13 at 174–216 MHz),
and the post-2017 incentive auction repack (which shrank UHF to 14–36) even
moved a few stations back into High-VHF. In the current FCC snapshot
(`fcc_tv_station.csv`), **1,841** stations operate on VHF channels 2–13
(alongside 6,229 on UHF 14–36). Note that a station's PSIP *virtual* channel
(e.g., `4.1` or `7.1`) often differs from its actual RF channel.

The **market dropdown** is built from the loaded data, not hard-coded: for each
state / district / territory it clusters the full-power TV stations by proximity,
ranks the clusters by station count, and lists the **top 5 markets**, each labeled
by its principal city and jumping to that cluster's **centroid** (the market's
geographic core). It stays correct as the data is refreshed.

## What it shows

For each channel it reports one of:

- **AVAILABLE** — no co-channel TV station's estimated protected zone reaches you.
- **ADJACENT** — co-channel is clear, but a first-adjacent (±1) station is close.
- **OCCUPIED** — you are inside a co-channel station's protected zone, or inside a
  point registration's keep-out (wireless mic, BAS link, MVPD/TV receive site,
  land-mobile base).
- **600 MHz** — channels 38–51, the repacked band now licensed to mobile carriers.
- **RESERVED** — channel 37 (radio astronomy / wireless medical telemetry).

A summary lists the white-space candidate channels and, of those, the ones the
MT300 is FCC-authorized to use.

## Method (and its limits)

- Every TV station (US / CA / MX, all services) is assigned an **estimated
  protected-contour radius** from its `erp_kw`, `height_aat` and band. A channel is
  **OCCUPIED** at your point if you fall within that radius **plus** the
  *co-channel margin* (default 14.4 km, the FCC fixed-device figure for low
  antennas; adjustable in the UI).
- The contour estimate is an **empirical approximation**, not the FCC propagation
  curves, so it can be optimistic or pessimistic near a contour edge. The co-channel
  margin and point keep-out are adjustable so you can see how sensitive a channel is.
- Point registrations (mics, BAS, MVPD/TV-receive, land-mobile) apply a simple
  radial keep-out on their channel(s).
- The 600 MHz file (`active_600Mhz_user.csv`) lists licensees by **frequency range,
  not tower location**, so channels 38–51 are flagged band-wide rather than
  geographically — consistent with the post-repack reality that those channels are
  no longer TV white space.

The tool is deliberately transparent: it always shows the nearest occupant and the
co/adjacent counts, so you can judge a verdict against the actual data.

## Data

[`data/`](data) is a verbatim mirror of <https://csv.wavedb.com/> (snapshot
**2026-11-22**), the public CSV exports of the FCC TV-bands protected-entity
databases. Files:

| File | What it is |
|---|---|
| `fcc_tv_station.csv` | TV stations (facility, channel, lat/lon, ERP, HAAT, service) — the dominant occupancy |
| `active_600Mhz_user.csv` | 600 MHz band licensees and their frequency ranges |
| `fixed_white_space_device.csv` (+ `_archive`) | registered fixed white-space devices (the registry an MT300 joins) |
| `licensed_low_power_auxiliary_station.csv` (+ `_archive`) | licensed wireless microphones / low-power aux |
| `permanent_bas_link.csv`, `temporary_bas_link.csv` (+ `_archive`) | broadcast auxiliary service links |
| `mvpd_receive_site.csv` (+ `_archive`) | cable (MVPD) receive sites |
| `tv_receive_site.csv` | TV translator/receive sites |
| `mobile_radio_base_station.csv` | land-mobile base stations (shared TV spectrum in some metros) |
| `medical_telemetry_service.csv` | wireless medical telemetry registrations |

The CSVs are refreshed daily at the source; re-download to update. They are public
records and are kept verbatim.

To refresh the mirror:

```bash
cd tools/whitespace-finder/data
for f in active_600Mhz_user fcc_tv_station fixed_white_space_device \
  fixed_white_space_device_archive licensed_low_power_auxiliary_station \
  licensed_low_power_auxiliary_station_archive medical_telemetry_service \
  mobile_radio_base_station mvpd_receive_site mvpd_receive_site_archive \
  permanent_bas_link temporary_bas_link temporary_bas_link_archive tv_receive_site; do
  curl -sSLO "https://csv.wavedb.com/$f.csv"
done
```
