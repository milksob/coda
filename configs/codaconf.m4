AC_CANONICAL_SYSTEM

dnl      --------  Adding a new system ----------
dnl Figure out what the GNU canonical name of your target is by
dnl running configure in the top directory
dnl   - add a configs/Makeconf.$sys file for your system
dnl   - add your VFS code to kernel-src/vfs/$vfsdir
dnl   - add a case statement below to set $sys and $vfsdir



case ${host_alias} in

	windows95 )
		sys=win95

 ;;
	nt )
		sys=cygwin32
 ;;
	cygwin32 )
		sys=cygwin32
 ;;

	*-*-netbsd* )
	    	shortsys=nbsd
	    	sys=i386_nbsd1
		vfsdir=bsd44
		os=`uname -r`
 ;;

	*-*-freebsd* )
		sys=i386_fbsd2
		vfsdir=bsd44
 ;;

	*-*-linux-* )
		shortsys=linux
		sys=linux
		case ${host_cpu} in
			i*6 ) 	arch=i386 ;;
			sparc ) arch=sparc ;;
			alpha ) arch=alpha ;;
		 esac
		fullos=`uname -r`
		case ${fullos} in
			2.0.* ) os=2.0 ; vfsdir=linux ;;
			2.1.* ) os=2.1 ; vfsdir=linux21 ;;
			2.2.* )	os=2.2 ; vfsdir=linux21 ;;
		esac
 ;;
esac
AC_SUBST(shortsys)
AC_SUBST(sys)
AC_SUBST(fullos)
AC_SUBST(os)
AC_SUBST(vfsdir)
