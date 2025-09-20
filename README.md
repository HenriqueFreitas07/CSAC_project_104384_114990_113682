<div id="top">

<!-- HEADER STYLE: CLASSIC -->
<div align="center">

# DetiCoins Mining

<em></em>

<!-- BADGES -->
<img src="https://img.shields.io/github/license/HenriqueFreitas07/CSAC_project_104384_114990_113682.git?style=default&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
<img src="https://img.shields.io/github/last-commit/HenriqueFreitas07/CSAC_project_104384_114990_113682.git?style=default&logo=git&logoColor=white&color=0080ff" alt="last-commit">
<img src="https://img.shields.io/github/languages/top/HenriqueFreitas07/CSAC_project_104384_114990_113682.git?style=default&color=0080ff" alt="repo-top-language">
<img src="https://img.shields.io/github/languages/count/HenriqueFreitas07/CSAC_project_104384_114990_113682.git?style=default&color=0080ff" alt="repo-language-count">

<!-- default option, no dependency badges. -->


<!-- default option, no dependency badges. -->

</div>
<br>

---

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
    - [Project Index](#project-index)
- [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
    - [Usage](#usage)
    - [Testing](#testing)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Overview
This repo lets basically, "mine" a made up coin, which is calculated with the generation of md5 hash over variations of the text "DETI_COINS_*" with a fixed size

## Project Structure

```sh
└── CSAC_project_104384_114990_113682.git/
    ├── AAD_A1-1.pdf
    ├── CSAC_project_104384_114990_113682.zip
    ├── README.md
    ├── cpu_utilities.h
    ├── cuda_driver_api_utilities.h
    ├── deti_coin_example.txt
    ├── deti_coins
    ├── deti_coins.c
    ├── deti_coins_cpu_MPI_OpenMP_search.h
    ├── deti_coins_cpu_OpenMP_search.h
    ├── deti_coins_cpu_avx2_search.h
    ├── deti_coins_cpu_avx_search.h
    ├── deti_coins_cpu_search.h
    ├── deti_coins_cpu_special_search.h
    ├── deti_coins_cuda_search.h
    ├── deti_coins_intel
    ├── deti_coins_mpi
    ├── deti_coins_vault.h
    ├── deti_coins_vault.txt
    ├── hello_cuda_driver_api.c
    ├── makefile
    ├── md5.h
    ├── md5_cpu.h
    ├── md5_cpu_avx.h
    ├── md5_cpu_avx2.h
    ├── md5_cpu_neon.h
    ├── md5_cuda.h
    ├── md5_cuda_kernel.cu
    ├── md5_test_data.h
    ├── presentation.pdf
    ├── search_utilities.h
    ├── suggestion_cuda_kernel.c
    ├── test_vault.bash
    ├── utils
    │   ├── OpenMP_4_threads.png
    │   ├── OpenMP_MPI_idea.jpg
    │   ├── README_MPI.md
    │   ├── README_OPENMP.md
    │   ├── README_webassembly.md
    │   ├── UML_MPI_OpenMP_comparison.png
    │   ├── Values.txt
    │   ├── bar_graphs_cuda.py
    │   ├── generate_histogram.py
    │   ├── generate_table_deti_coin_power.py
    │   ├── gpu_attempts_120s.png
    │   ├── graphs
    │   ├── line_graphs_attempts_120s.py
    │   ├── requirements.txt
    │   └── suggestion.c
    ├── web_assembly.c
    ├── web_assembly.html
    ├── web_assembly.js
    └── web_assembly.wasm
```

### Project Index

<details open>
	<summary><b><code>CSAC_PROJECT_104384_114990_113682.GIT/</code></b></summary>
	<!-- __root__ Submodule -->
	<details>
		<summary><b>__root__</b></summary>
		<blockquote>
			<div class='directory-path' style='padding: 8px 0; color: #666;'>
				<code><b>⦿ __root__</b></code>
			<table style='width: 100%; border-collapse: collapse;'>
			<thead>
				<tr style='background-color: #f8f9fa;'>
					<th style='width: 30%; text-align: left; padding: 8px;'>File Name</th>
					<th style='text-align: left; padding: 8px;'>Summary</th>
				</tr>
			</thead>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/md5_cuda.h'>md5_cuda.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ md5 hashing code </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/deti_coins_cuda_search.h'>deti_coins_cuda_search.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ Code for searching coins on the CUDA cores </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/deti_coins_cpu_MPI_OpenMP_search.h'>deti_coins_cpu_MPI_OpenMP_search.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ Code for searching coins with the use of MPI, for parallel processes.  </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/cpu_utilities.h'>cpu_utilities.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ General measuring and benchmarking functions </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/md5_cpu_avx.h'>md5_cpu_avx.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ Code for searching coins with the use of AVX registers, for parallel mining. </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/deti_coins_vault.h'>deti_coins_vault.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ Code to save the deti coins. </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/deti_coins_cpu_avx2_search.h'>deti_coins_cpu_avx2_search.h</a></b></td>
					<td style='padding: 8px;'>Code>❯  Code for searching coins with the use of AVX2 (256 bit range) registers, for parallel mining.  </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/deti_coins_cpu_OpenMP_search.h'>deti_coins_cpu_OpenMP_search.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ Code for searching deticoins with the use of multithreaded processes for more efficient mining </code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/cuda_driver_api_utilities.h'>cuda_driver_api_utilities.h</a></b></td>
					<td style='padding: 8px;'>Code>❯ General CUDA functions for simplification.</code></td>
				</tr>
			</table>
		</blockquote>
	</details>
	<!-- utils Submodule -->
	<details>
		<summary><b>utils</b></summary>
		<blockquote>
			<div class='directory-path' style='padding: 8px 0; color: #666;'>
				<code><b>⦿ utils</b></code>
			<table style='width: 100%; border-collapse: collapse;'>
			<thead>
				<tr style='background-color: #f8f9fa;'>
					<th style='width: 30%; text-align: left; padding: 8px;'>File Name</th>
					<th style='text-align: left; padding: 8px;'>Summary</th>
				</tr>
			</thead>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/requirements.txt'>requirements.txt</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/bar_graphs_cuda.py'>bar_graphs_cuda.py</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/Values.txt'>Values.txt</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/generate_table_deti_coin_power.py'>generate_table_deti_coin_power.py</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/generate_histogram.py'>generate_histogram.py</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/suggestion.c'>suggestion.c</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
				<tr style='border-bottom: 1px solid #eee;'>
					<td style='padding: 8px;'><b><a href='https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/master/utils/line_graphs_attempts_120s.py'>line_graphs_attempts_120s.py</a></b></td>
					<td style='padding: 8px;'>Code>❯ REPLACE-ME</code></td>
				</tr>
			</table>
		</blockquote>
	</details>
</details>

---

## Getting Started

### Prerequisites

This project requires the following dependencies:

- **Programming Language:** C

### Installation

Build CSAC_project_104384_114990_113682.git from the source and intsall dependencies:

1. **Clone the repository:**

    ```sh
    ❯ git clone https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git
    ```

2. **Navigate to the project directory:**

    ```sh
    ❯ cd CSAC_project_104384_114990_113682.git
    ```


## Contributing

- **💬 [Join the Discussions](https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/discussions)**: Share your insights, provide feedback, or ask questions.
- **🐛 [Report Issues](https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/issues)**: Submit bugs found or log feature requests for the `CSAC_project_104384_114990_113682.git` project.
- **💡 [Submit Pull Requests](https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git/blob/main/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your github account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone https://github.com/HenriqueFreitas07/CSAC_project_104384_114990_113682.git
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to github**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!
</details>


---

## License

Csac_project_104384_114990_113682.git is protected under the [LICENSE](https://choosealicense.com/licenses/mit) License. For more details, refer to the [LICENSE](https://choosealicense.com/licenses/mit) file.

---



---
