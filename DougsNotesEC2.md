

## EC2 Links ##
  * [Service Overview](http://aws.amazon.com/ec2/)
  * [Instance Pricing](http://aws.amazon.com/ec2/#pricing)
  * [Instance Types](http://docs.amazonwebservices.com/AWSEC2/2008-02-01/DeveloperGuide/instance-types.html)
  * [Public EC2 AMI Directory](http://developer.amazonwebservices.com/connect/kbcategory.jspa?categoryID=101)
  * [Developer's Guide](http://docs.amazonwebservices.com/AWSEC2/2008-02-01/DeveloperGuide/index.html)
  * [Management Console](https://console.aws.amazon.com/ec2/home)

## How to Create an EC2 Instance ##

The following steps got me up and running with a machine on EC2.  The following command created the machine instance.  The `ami-5c709435` parameter refers to a public AMI.  Check out the [Public EC2 AMI Directory](http://developer.amazonwebservices.com/connect/kbcategory.jspa?categoryID=101) for other images.  The `-k gsg-keypair` argument refers to the keypair generated with a previous run of the command `ec2-add-keypair gsg-keypair`.  See [Amazon EC2 Getting Started Guide](http://docs.amazonwebservices.com/AWSEC2/latest/GettingStartedGuide/) for help getting setup.  I cut and pasted the private key into the file `~/.ec2/id_rsa-gsg-keypair`

```
$ ec2-run-instances ami-5c709435 -k gsg-keypair [-t m1.large]
RESERVATION               r-9c5aeaf5            724784214682 default
INSTANCE                  i-ccf678a5            ami-5c709435            pending gsg-keypair     0               m1.small 2009-02-03T05:50:57+0000 us-east-1a aki-a71cf9ce ari-a51cf9cc 
```

After waiting a bit, I ran the following command to obtain the instance ID and hostname.

```
$ ec2-describe-instances 
RESERVATION               r-9c5aeaf5                 724784214682                       default
INSTANCE                  i-ccf678a5                 ami-5c709435                       ec2-75-101-254-105.compute-1.amazonaws.com domU-12-31-38-00-6C-78.compute-1.internal running gsg-keypair 0  m1.small 2009-02-03T05:50:57+0000 us-east-1a aki-a71cf9ce ari-a51cf9cc 
```

Copy my account tarball to the machine:

```
scp -i .ec2/id_rsa-gsg-keypair ec2/doug-ec2-account.tgz root@ec2-75-101-254-105.compute-1.amazonaws.com:/tmp
```

I was then able to log into the system as root without a password with the following comand:

```
$ ssh -i .ec2/id_rsa-gsg-keypair root@ec2-75-101-254-105.compute-1.amazonaws.com
```

Then, after logging into the machine, setup the doug account:

```
# useradd -d /home/doug -s /bin/bash doug
# echo "doug    ALL=(ALL) ALL" >> /etc/sudoers
# mkdir /mnt/hypertable
# chmod a+rwx /mnt/hypertable 
# passwd doug
# su - doug
$ tar xzvf /tmp/doug-bash-account-setup.tgz 
```

## Creating a new Image ##

  1. Start from an existing instance
```
$ ec2-run-instances -t m1.large ami-0be90e62 -k gsg-keypair
...
$ ec2-describe-instances 
RESERVATION	r-7b45ce12	724784214682	default
INSTANCE	i-20721b49	ami-0be90e62	ec2-67-202-13-144.compute-1.amazonaws.com	domU-12-31-39-01-64-51.compute-1.internal	running	gsg-keypair	0		m1.large	2009-04-19T17:54:58+0000	us-east-1c
...
$ ssh -i .ec2/id_rsa-gsg-keypair root@ec2-67-202-13-144.compute-1.amazonaws.com
```
  1. Make modifications, but be sure to clean up afterwards.  The /mnt directory does not get bundled into the new images, so that's good place to put temporary files.
  1. As per the the [ami-0be90e62 instructions](http://developer.amazonwebservices.com/connect/entry!default.jspa?externalID=2057) do the following:
```
$ depmod -a; modprobe loop
$ echo '127.0.0.1 localhost.localdomain localhost' > /etc/hosts
$ echo "RUBYLIB=/usr/lib/site_ruby; export RUBYLIB" >> ~/.bash_profile
$ source ~/.bash_profile
```
  1. Copy the .pem files from laptop to the virtual machine:
```
scp -i .ec2/id_rsa-gsg-keypair .ec2/*.pem root@ec2-67-202-13-144.compute-1.amazonaws.com:/mnt
```
  1. On the VM, bundle the new volume
```
$ ec2-bundle-vol -k /mnt/pk-*.pem -c /mnt/cert-*.pem -u 180777447352 --arch x86_64
```
  1. Create a bucket in S3 to hold the image.  Use S3 Firefox Organizer to create the bucket.
  1. On the VM, upload the bundle to S3 (assuming bucket /hypertable/ami/hypertable-0.9.2.3-centos)
```
ec2-upload-bundle -b /hypertable/ami/hypertable-0.9.2.3-centos -m /tmp/image.manifest.xml -a 1DYT... -s P6Tl...
```
  1. Register the image.  On laptop, execute the following command:
```
$ ec2-register /hypertable/ami/hypertable-0.9.2.3-centos/image.manifest.xml
IMAGE	ami-5c56b135
```
  1. Make it public
```
$ ec2-modify-image-attribute ami-5c56b135 --launch-permission -a all
```

## Latest AMI ##
ami-2f40a646

## S3 ##

Here's a simple and easy to setup tool for working with s3:

[aws tool](http://www.timkay.com/aws/)

Here's how to copy a file to s3:

```
./aws put hypertable-users/foo/bar.tar.bz2 bar.tar.bz2
```

and here's how to copy a file from s3:

```
./aws get hypertable-users/foo/bar.tar.bz2 .
```