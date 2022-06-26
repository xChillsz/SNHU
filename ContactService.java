package contactService;

import java.util.ArrayList;

public class ContactService {
	
	//This holds a list of lists called contacts created using Contact Service.
	private ArrayList<Contact> contacts;
	
	public ContactService() {
		//creates a list to add to contacts list;
		contacts = new ArrayList<>();
	}
	
	
	//Method used to add contacts to list if they're not present within our list of lists
	public boolean addContact(Contact contact) {
		//Used to check if Contact exists.
		boolean isPresent = false;
		//If the contactId is length is too long then nothing will be created and the loop will not be initiated.
		if(contact.contactId.length()>10){
			return false;
		}
			else {
			//iterates through our lists of Contacts
			for(Contact contactList:contacts) {
				if(contactList.equals(contact)) {
					//Sets isPresent variable to true if it does exist
					isPresent = true;
				}
			}
			//If the contact does not exist add the contact then return True
			if(!isPresent) {
				//Operation from Contact Class used to Add New Contact
				contacts.add(contact);
				return true;
			}
			else {
				return false;
			}
			}
	}
	
	
	//Operation used to Delete a Contact using contactId, if the contactId is present in our Array.
	public boolean deleteContact(String contactId) {
		//Iterates through our list of Contacts
		for(Contact contactList:contacts) {
			//if the current item's contactId = the input contactId it will remove the item & return true otherwise it will return false
			if(contactList.getContactId().equals(contactId)) {
				contacts.remove(contactList);
				return true;
			}
		}
		return false;
	}
	
	
	//Used to Find a Contact w/ a matching contactId and update the Contact Information provided.
	public boolean updateContact(String contactId, String firstName, String lastName, String phoneNumber, String address) {
		//Iterates through our Contact List to compare entered contactId & update it if it exists
		for(Contact contactList:contacts) {
			//Checks Contact Id
			if(contactList.getContactId().equals(contactId)) {
				//Validation to ensure entry isn't null or longer than expected.
				if(!firstName.equals("") && !(firstName.length()>10)) {
					contactList.setFirstName(firstName);
				}
				
				//Validation to ensure entry isn't null or longer than expected.
				if(!lastName.equals("") && !(lastName.length()>10)) {
					contactList.setLastName(lastName);
				}
				
				//Validation to ensure entry isn't null or longer than expected.
				if(!phoneNumber.equals("") && !(phoneNumber.length()==10)) {
					contactList.setPhoneNumber(phoneNumber);
				}
				
				//Validation to ensure entry isn't null or longer than expected.
				if(!address.equals("") && !(address.length()>30)) {
					contactList.setAddress(address);
				}
				
				return true;
			}
		}
		return false;
	}
	
	
}
