# Team 42

## Target Environment

| Item         | Version |
| ------------ | ------- |
| OS           | Mac     |
| Toolchain    | cmake   |
| C++ Standard | C++17   |

### Additional Build Instructions

No additional instructions required.

## Team Members

| Name                   | Mobile   | Email                 | Development OS/Toolchain |
| ---------------------- | -------- | --------------------- | ------------------------ |
| Loh Wei Kiat           | 81125027 | loh.weikiat@u.nus.edu | Mac/cmake                |
| Tay Sheryl             | ________ | e0323809@u.nus.edu    | Mac/cmake                |
| Lee Hui Ting           | ________ | huiting.lee@u.nus.edu | Mac/cmake                |
| Foo Chuan Le, Nicholas | 93259928 | e0303296@u.nus.edu    | Windows/cmake            |
| Lim Haw Jia            | 96992328 | hawjia.lim@u.nus.edu  | Ubuntu/cmake             |

## Conventions

### Branching

This repo contains two main branches.

- `master`:  contains submission-ready code
- `develop`: contains pre-submission code

Any development should be done in a seperate feature branch created from `develop`. 

Branch naming convention is as follows:

```
<group>_<issue_no>_<feature>  // e.g. feat_11_pkb_follows_api
```

- `group`: type of work being done in the branch, e.g. `feat`, `bugfix`, `refactor`, `testing`
- `issue_no`: the github issue number that corresponds to the branch
- `feature`: short description of the work being done in the branch

---

> Please fill in this Readme with the required information, and delete this box.
>
> 1. Note that if you are using the `Windows Startup SPA Solution`, your target environment must be using `Microsoft Visual Studio Enterprise 2019`.
>    If you are using `Cross-platform Startup SPA Solution`, you will need to decide on your target environment (see [Wiki](https://github.com/nus-cs3203/project-wiki/wiki/Version-Control-System-and-Code-Repository)).
>    Then, please enter the appropriate information below, giving details on the full toolchain (ie. OS, cmake and make versions.)
> 1. The `.gitigore` is already pre-configured, note that the included libraries files (ie. `Team00/Code00/lib`) must accompany this repository and not to be removed from the repository.
> 1. Put your test cases inside the correct directory (ie. `Team00/Tests00/`) not inside the scratch test directory (ie. `Team00/Code00/tests/`). The `.gitignore` is configured to prevent this.
> 1. If you encounter build issues, please visit the respective FAQ *before* reaching out for help:
>    1. [Windows-Startup-SPA-Solution FAQ](https://github.com/nus-cs3203/project-wiki/wiki/Windows-Startup-SPA-Solution#faq)
>    1. [Cross-platform-Startup-SPA-Solution FAQ](https://github.com/nus-cs3203/project-wiki/wiki/Cross-platform-Startup-SPA-Solution#faq)
> 1. Avoid using removed/deprecated features in C++, make sure you use features that exist in C++17. It is not recommended to use an earlier version of C++, if you want to proceed, please specify the C++ version specifically in the build configuration.