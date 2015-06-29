
# Get the Hypertable source code #

We're using the [git](http://git.or.cz/) version control system to manage the source code for this project.  To obtain a local copy of the repository, install the latest version of git (at least 1.5.3.7) and then issue the following commands:
```
git config --global user.name "<your name>"
git config --global user.email "<your email>"
```

to configure your identity in the commits. To obtain the source:

```
cd ~/src
git clone git://github.com/hypertable/hypertable.git
```


This git repository represents the most up-to-date version of the source code.  We push changes into this repository only if all of the regression tests pass.  To update your local git repository (created with the above command) with the latest changes, issue the following commands:
```
cd ~/src/hypertable
git pull

```

# Become a contributor #

There are a few options to contribute to the project. Email patches always work (see the documentation by git send-email --help). Regular contributor might find a public repository hosting service helpful. For example, repo.or.cz, unfuddle.com and github.com all offer free git hosting, enough for hypertable. The following instruction is for github.com (as it's the easiest to fork a project):

  1. Create an account on github.com (just follow the easy instructions :)
  1. Goto http://github.com/hypertable/hypertable/tree and click the fork button besides the project name.
  1. Change directory to your local repository (assuming you've done "Get the Hypertable source code"), issue the following command:
```
git remote add github <your_push_url>
```
  1. Work on awesome features
  1. Before sending out merge/pull requests, merge (rebase is recommended if the working branch is behind master for more than 10 commits or so, which makes it hard to visualize the branch) against the official master branch
  1. You can then push your changes (assuming it's in the topic/name branch) to this public repository:
```
git push github topic/name
```
  1. Tell everyone about your repo: `http://github.com/<your_user_name>/hypertable/tree`

# Browse the wiki (documentation) source and history #
http://code.google.com/p/hypertable/source/browse/wiki