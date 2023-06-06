// Code generated by "stringer -type InfoCode -linecomment"; DO NOT EDIT.

package adbc

import "strconv"

func _() {
	// An "invalid array index" compiler error signifies that the constant values have changed.
	// Re-run the stringer command to generate them again.
	var x [1]struct{}
	_ = x[InfoVendorName-0]
	_ = x[InfoVendorVersion-1]
	_ = x[InfoVendorArrowVersion-2]
	_ = x[InfoDriverName-100]
	_ = x[InfoDriverVersion-101]
	_ = x[InfoDriverArrowVersion-102]
	_ = x[InfoDriverADBCVersion-103]
}

const (
	_InfoCode_name_0 = "VendorNameVendorVersionVendorArrowVersion"
	_InfoCode_name_1 = "DriverNameDriverVersionDriverArrowVersionDriverADBCVersion"
)

var (
	_InfoCode_index_0 = [...]uint8{0, 10, 23, 41}
	_InfoCode_index_1 = [...]uint8{0, 10, 23, 41, 58}
)

func (i InfoCode) String() string {
	switch {
	case i <= 2:
		return _InfoCode_name_0[_InfoCode_index_0[i]:_InfoCode_index_0[i+1]]
	case 100 <= i && i <= 103:
		i -= 100
		return _InfoCode_name_1[_InfoCode_index_1[i]:_InfoCode_index_1[i+1]]
	default:
		return "InfoCode(" + strconv.FormatInt(int64(i), 10) + ")"
	}
}
