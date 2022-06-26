package contactService;

public class Contact {
	
	String contactId;
	String firstName;
	String lastName;
	String phoneNumber;
	String address;

//Constructor w/ Params

public Contact(String contactId, String firstName, String lastName, String phoneNumber, String address) {
	super();
	this.contactId = contactId;
	this.firstName = firstName;
	this.lastName = lastName;
	this.phoneNumber = phoneNumber;
	this.address = address;
}


// Getters

public String getContactId() {
	return contactId;
}

public String getFirstName() {
	return firstName;
}

public String getLastName() {
	return lastName;
}

public String getPhoneNumber() {
	return phoneNumber;
}

public String getAddress() {
	return address;
}

//Setters

public void setContactId(String contactId) {
	this.contactId = contactId;
}

public void setFirstName(String firstName) {
	this.firstName = firstName;
}

public void setLastName(String lastName) {
	this.lastName = lastName;
}

public void setPhoneNumber(String phoneNumber) {
	this.phoneNumber = phoneNumber;
}

public void setAddress(String address) {
	this.address = address;
}

//Used to compare entries

@Override

public String toString() {
return "Contact || ContactId = " + contactId + " | firstName = " + firstName + " | lastName = " + lastName + " | phoneNumber = " + phoneNumber + " | address = " + address + " ||";
}
}
