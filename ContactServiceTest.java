package contactService;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;

class ContactServiceTest {

	//Testing Add Method
	@Test
	public void testPassContactAdd() {
		//Calls the Contact Service Operation to start an instance up.
		ContactService cs = new ContactService();
		
		//This test should pass
		Contact c1 = new Contact("ZZZZZZZZ01","Johnathane","Anguianooo","1234567890","300 New Road Mayslanding NuJrz");
		assertEquals(true,cs.addContact(c1));
		
	}
	@Test
	public void testFailContactAdd() {
		//Calls the Contact Service Operation to start an instance up.
		ContactService cs = new ContactService();
		//This contact has 11 Characters in the ContactId
		Contact c2 = new Contact("ZZZZZZZZ011","Johnathane","Anguianooo","1234567890","300 New Road Mayslanding NuJrz");
		assertEquals(false,cs.addContact(c2));
	}

	@Test
	public void testDelete() {
		
		ContactService cs = new ContactService();
		
		
		//Creating a few Contact Vars
		Contact c1 = new Contact("ZZZZZZZZ01","Johnathane","Angguianoo","1234567890","300 New Road Mayslanding NuJrz");
		Contact c2 = new Contact("ZZZZZZZZ02","Fohnathane","Anghuianoo","1234567891","301 New Road Mayslanding NuJrz");
		Contact c3 = new Contact("ZZZZZZZZ03","Pohnathane","Anguianhoo","1234567892","302 New Road Mayslanding NuJrz");
			
		
			
		//Adding the contacts
		cs.addContact(c1);
		cs.addContact(c2);
		cs.addContact(c3);
			
			
		//Assertions - All Use ContactId to Find/Delete Contact
			
		//1. Will Be Deleted  
		assertEquals(true,cs.deleteContact("ZZZZZZZZ01"));
		
		//2. Will not Be Deleted (Does not Exist)
		assertEquals(false,cs.deleteContact("ZZZZZZZZ04"));

		//3. This was deleted in Test 1 (Does Not Exist)
		assertEquals(false,cs.deleteContact("ZZZZZZZZ01"));
		}
		
		
		@Test
		public void testUpdate() {
			ContactService cs = new ContactService();
			
			//Creating a few Contact Vars
			Contact c1 = new Contact("ZZZZZZZZ01","Johnathane","Angguianoo","1234567890","300 New Road Mayslanding NuJrz");
			Contact c2 = new Contact("ZZZZZZZZ02","Fohnathane","Anghuianoo","1234567891","301 New Road Mayslanding NuJrz");
			Contact c3 = new Contact("ZZZZZZZZ03","Pohnathane","Anguianhoo","1234567892","302 New Road Mayslanding NuJrz");
			
			//Adding the contacts
			cs.addContact(c1);
			cs.addContact(c2);
			cs.addContact(c3);
			
			//Assertions - All Use ContactId to Find/Update Contact
			//1. Will Be Updated  
			assertEquals(true,cs.updateContact("ZZZZZZZZ01","Johnathang","Angguianoo","1234565555","303 New Road Mayslanding NuJrz"));
			
			//2. Will not Be Updated (Does not Exist)
			assertEquals(false,cs.updateContact("ZZZZZZZZ04","Johnathane","Angguianoo","1234567890","300 New Road Mayslanding NuJrz"));
			
			//3. Will not be updated b/c Nulls
			cs.updateContact("ZZZZZZZZ02","","Angguianoo","1234567890","300 New Road Mayslanding NuJrz");
			assertEquals(false,c1.getFirstName().equals(""));
			cs.updateContact("ZZZZZZZZ02","Johnathane","","1234567890","300 New Road Mayslanding NuJrz");
			assertEquals(false,c1.getLastName().equals(""));
			cs.updateContact("ZZZZZZZZ02","Johnathane","Angguianoo","","300 New Road Mayslanding NuJrz");
			assertEquals(false,c1.getAddress().equals(""));
			cs.updateContact("ZZZZZZZZ02","Johnathane","Angguianoo","1234567890","");
			assertEquals(false,c1.getPhoneNumber().equals(""));
		}


}
