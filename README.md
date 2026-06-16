# Ackland-Jones Analysis

Identifies FCC, HCP, BCC and ICO crystal structures using adaptive nearest-neighbor bond-angle analysis — no cutoff parameter required (Ackland & Jones, PRB 2006).

## Install

```bash
vpm install @voltlabs/ackland-jones
```

## CLI

```bash
ackland-jones <input_dump> [output_base]
```

| Argument | Required | Default | Description |
|---|---|---|---|
| `<input_dump>` | yes | — | Input LAMMPS dump. |
| `[output_base]` | no | derived from input | Base path for output files. |

This plugin takes no tuning flags — structure classification is parameter-free.

## Exports

| Output file | Exposure | Exporter → artifact |
|---|---|---|
| `{output_base}_atoms.parquet` | Ackland-Jones Structure | AtomisticExporter → glb |
| `{output_base}_ackland_jones.parquet` | Ackland-Jones Summary | — (listing-only) |

---

Full input contract and examples: https://docs.voltcloud.dev/docs/plugins
