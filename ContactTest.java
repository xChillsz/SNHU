package contactService;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;

class ContactTest {
	//Creating Class Object to access methods.
	Contact contact = new Contact("ZZZZZZZZ01","Johnathang","Angguianoo","1234565555","303 New Road Mayslanding NuJrz");
	
	@Test
	void getContactId() {
		assertEquals("ZZZZZZZZ01", contact.getContactId());
	}
	
	@Test
	void getFirstName() {
		assertEquals("Johnathang", contact.getFirstName());
	}
	
	@Test
	void getLastName() {
		assertEquals("Angguianoo", contact.getLastName());
	}
	
	@Test
	void getPhoneNumber() {
		assertEquals("1234565555", contact.getPhoneNumber());
	}
	
	@Test
	void getAddress() {
		assertEquals("303 New Road Mayslanding NuJrz", contact.getAddress());
	}
	
	@Test
	void testToString() {
		assertEquals("Contact || ContactId = ZZZZZZZZ01 | firstName = Johnathang | lastName = Angguianoo | phoneNumber = 1234565555 | address = 303 New Road Mayslanding NuJrz ||", contact.toString());
		
	}
}
